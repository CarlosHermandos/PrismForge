#pragma once
#include "Assets.h"
#include "Effects.h"
#include "Hybrid.h"
#include "Parameters.h"
#include "Sequencer.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
namespace prism
{
constexpr double pi = 3.14159265358979323846;
struct Settings
{
    float timbre = .6f, attack = .01f, decay = .2f, sustain = .7f, release = .3f, cutoff = 8000,
          resonance = .15f, gain = .5f;
    std::array<float, paramCount - 8> extra = []
    {
        std::array<float, paramCount - 8> a{};
        for (size_t i = 8; i < paramCount; ++i)
            a[i - 8] = parameters[i].def;
        return a;
    }();
    float &operator[](size_t i) noexcept
    {
        switch (i)
        {
        case 0:
            return timbre;
        case 1:
            return attack;
        case 2:
            return decay;
        case 3:
            return sustain;
        case 4:
            return release;
        case 5:
            return cutoff;
        case 6:
            return resonance;
        case 7:
            return gain;
        default:
            return extra[i - 8];
        }
    }
    float operator[](size_t i) const noexcept
    {
        return const_cast<Settings *>(this)->operator[](i);
    }
};
struct Tables
{
    static constexpr int size = 2048, levels = 10;
    std::array<std::array<std::array<float, size + 1>, levels>, 4> data{};
    Tables()
    {
        for (int f = 0; f < 4; ++f)
            for (int l = 0; l < levels; ++l)
            {
                auto &t = data[f][l];
                int h = 1 << l;
                for (int i = 0; i < size; ++i)
                {
                    double p = 2 * pi * i / size, v = 0;
                    for (int k = 1; k <= h; ++k)
                    {
                        double a = 0;
                        if (f == 0)
                            a = k == 1 ? 1 : 0;
                        if (f == 1 && k % 2)
                            a = ((k % 4) == 1 ? 1.0 : -1.0) / (k * k) * 8 / (pi * pi);
                        if (f == 2)
                            a = 2 / (pi * k) * (k % 2 ? 1 : -1);
                        if (f == 3 && k % 2)
                            a = 4 / (pi * k);
                        v += a * std::sin(p * k);
                    }
                    t[i] = float(v);
                }
                t[size] = t[0];
            }
    }
    float read(int l, double phase, float frame) const noexcept
    {
        float x = float(phase * size);
        int i = std::min(size - 1, int(x));
        float frac = x - i;
        float f = std::clamp(frame, 0.f, 1.f) * 3;
        int a = std::min(2, int(f));
        float mix = f - a;
        auto sample = [&](int w)
        {
            auto &t = data[w][l];
            return t[i] + frac * (t[i + 1] - t[i]);
        };
        return sample(a) + (sample(a + 1) - sample(a)) * mix;
    }
};
struct Envelope
{
    float value = 0, progress = 0, initial = 0;
    int stage = 0;
    void start(bool restart)
    {
        if (restart)
            value = 0;
        initial = value;
        progress = 0;
        stage = 1;
    }
    void stop()
    {
        if (stage && stage != 4)
        {
            initial = value;
            progress = 0;
            stage = 4;
        }
    }
    float tick(float a, float d, float s, float r, double sr, float ac = 0, float dc = 0,
               float rc = 0) noexcept
    {
        if (stage == 1)
        {
            progress += float(1 / (std::max(.001f, a) * sr));
            value = initial + (1 - initial) * std::pow(std::min(1.f, progress), std::exp2(ac * 2));
            if (progress >= 1)
            {
                value = 1;
                progress = 0;
                stage = 2;
            }
        }
        else if (stage == 2)
        {
            progress += float(1 / (std::max(.001f, d) * sr));
            value = 1 - (1 - s) * std::pow(std::min(1.f, progress), std::exp2(dc * 2));
            if (progress >= 1)
            {
                value = s;
                stage = 3;
            }
        }
        else if (stage == 3)
            value += float(1 - std::exp(-1 / (.003 * sr))) * (s - value);
        else if (stage == 4)
        {
            progress += float(1 / (std::max(.001f, r) * sr * std::max(.001f, initial)));
            value = initial * (1 - std::pow(std::min(1.f, progress), std::exp2(rc * 2)));
            if (progress >= 1)
            {
                value = 0;
                stage = 0;
            }
        }
        return value;
    }
};
struct Filter
{
    float z1 = 0, z2 = 0;
    float process(float input, float cutoff, float resonance, int type, float drive,
                  double sr) noexcept
    {
        float gain = dbGain(drive),
              x = drive > .001f ? std::tanh(input * gain) / std::sqrt(gain) : input;
        float g = float(std::tan(pi * std::clamp(double(cutoff), 20., sr * .45) / sr)),
              k = 2 - 1.8f * std::clamp(resonance, 0.f, 1.f), a = 1 / (1 + g * (g + k));
        float v3 = x - z2, v1 = a * z1 + g * a * v3, v2 = z2 + g * a * z1 + g * g * a * v3;
        z1 = 2 * v1 - z1;
        z2 = 2 * v2 - z2;
        if (std::abs(z1) < 1e-20f)
            z1 = 0;
        if (std::abs(z2) < 1e-20f)
            z2 = 0;
        switch (type)
        {
        case 1:
            return x - k * v1 - v2;
        case 2:
            return v1;
        case 3:
            return x - k * v1;
        default:
            return v2;
        }
    }
};
struct Voice
{
    int note = -1, channel = 1;
    bool held = false, pendingRelease = false;
    uint64_t age = 0;
    std::array<Envelope, 4> envelope{};
    std::array<HybridVoice, 3> hybrid{};
    std::array<std::array<double, 7>, 3> phase{};
    std::array<std::array<int, 7>, 3> level{}, oldLevel{};
    std::array<std::array<float, 7>, 3> blend{};
    std::array<double, 10> lfo{};
    std::array<bool, 10> lfoDone{};
    std::array<std::array<Filter, 2>, 2> filter{};
    double subPhase = 0;
    float pitch = 60, pitchStep = 0;
    int glideLeft = 0;
    float velocity = 0, releaseVelocity = 0, random = 0, noiseState = 0;
    Stereo last{}, residual{};
};
class Engine
{
    Tables tables;
    std::array<Voice, 16> voices{};
    std::array<bool, 16> pedal{};
    std::array<float, 16> bend{}, wheel{}, pressure{}, timbreCC{};
    struct Held
    {
        int note = -1, channel = 1, velocity = 0;
        uint64_t age = 0;
        bool down = false, sustained = false;
    };
    std::array<Held, 256> heldNotes{};
    uint64_t serial = 0;
    uint32_t rng = 1979, configuredSeed = 1979;
    double rate = 48000;
    float alpha = .004f, residualDecay = .99f, lastPitch = 60;
    bool hasLastPitch = false, wasMono = false;
    std::array<float, paramCount> smooth{};
    std::array<double, 10> globalLfo{};
    std::array<float, 30> sources{};
    Effects effects;
    Sequencer sequencer;
    float random() noexcept
    {
        rng ^= rng << 13;
        rng ^= rng >> 17;
        rng ^= rng << 5;
        return float(rng & 0xffffff) / 8388607.5f - 1;
    }
    void releaseVoice(Voice &v, float velocity)
    {
        v.held = false;
        v.releaseVelocity = velocity;
        bool oneShot = false;
        for (int o = 0; o < 3; ++o)
        {
            int mode = int(settings[size_t(hybrid1_mode + o * 18)]);
            oneShot =
                oneShot ||
                ((mode == 1 || mode == 2) && settings[size_t(hybrid1_oneShot + o)] > .5f &&
                 settings[size_t(hybrid1_loop + o * 18)] < .5f && !v.hybrid[size_t(o)].finished &&
                 settings[size_t(oscillatorIndices[size_t(o)].level)] > 0);
        }
        v.pendingRelease = oneShot;
        for (size_t e = 0; e < v.envelope.size(); ++e)
            if (e != 0 || !oneShot)
                v.envelope[e].stop();
    }
    void pitchTarget(Voice &v, int note, bool glide)
    {
        float start = v.pitch;
        v.note = note;
        v.glideLeft = glide ? int(settings[prism::glide] * rate) : 0;
        if (v.glideLeft > 0)
            v.pitchStep = (note - start) / v.glideLeft;
        else
            v.pitch = float(note);
    }
    void startVoice(Voice &v, int channel, int note, int velocity, bool legato)
    {
        if (!legato)
        {
            auto residual = v.last + v.residual;
            float previous = v.pitch;
            v = Voice{};
            v.residual = residual;
            v.pitch = hasLastPitch ? lastPitch : float(note);
            if (settings[mono] > .5f && hasLastPitch)
                v.pitch = previous;
            v.random = random();
            for (int o = 0; o < 3; ++o)
            {
                auto idx = oscillatorIndices[size_t(o)];
                for (int u = 0; u < 7; ++u)
                {
                    v.phase[size_t(o)][size_t(u)] =
                        std::fmod(settings[size_t(idx.phase)] +
                                      (random() + 1) * .5f * settings[size_t(idx.random)],
                                  1.);
                    v.blend[size_t(o)][size_t(u)] = 1;
                }
            }
            for (int i = 0; i < 10; ++i)
                v.lfo[size_t(i)] = settings[size_t(lfo1_phase + i * 8)];
            for (auto &e : v.envelope)
                e.start(true);
        }
        else if (settings[retrigger] > .5f)
            for (auto &e : v.envelope)
                e.start(true);
        v.channel = channel;
        v.velocity = velocity / 127.f;
        v.held = true;
        v.age = ++serial;
        pitchTarget(v, note, hasLastPitch);
        lastPitch = float(note);
        hasLastPitch = true;
    }
    void monoFallback()
    {
        Held *newest = nullptr;
        for (auto &h : heldNotes)
            if (h.note >= 0 && (h.down || h.sustained) && (!newest || h.age > newest->age))
                newest = &h;
        if (newest)
        {
            auto &v = voices[0];
            if (v.note != newest->note || v.channel != newest->channel)
                startVoice(v, newest->channel, newest->note, newest->velocity, true);
        }
        else
            releaseVoice(voices[0], 0);
    }
    static float shape(double phase, int type) noexcept
    {
        switch (type)
        {
        case 1:
            return float(1 - 4 * std::abs(phase - .5));
        case 2:
            return float(2 * phase - 1);
        case 3:
            return phase < .5 ? 1.f : -1.f;
        case 4:
            return float(1 - 2 * phase);
        default:
            return float(std::sin(phase * 2 * pi));
        }
    }

  public:
    bool externallyClockedAssets = false;
    AssetPool assets;
    Settings settings;
    double tempo = 120;
    bool transportPlaying = false;
    void prepare(double sr)
    {
        rate = std::isfinite(sr) ? std::clamp(sr, 8000., 192000.) : 48000.;
        for (size_t i = 0; i < paramCount; ++i)
            smooth[i] = settings[i];
        alpha = float(1 - std::exp(-1 / (.005 * rate)));
        residualDecay = float(std::exp(-1 / (.003 * rate)));
        effects.prepare(rate);
        sequencer.reset(uint32_t(settings[seed]));
        sequencer.configure(settings);
        rng = uint32_t(settings[seed]);
        reset();
    }
    void reset() noexcept
    {
        voices = {};
        pedal = {};
        bend = {};
        wheel = {};
        pressure = {};
        timbreCC = {};
        heldNotes = {};
        globalLfo = {};
        hasLastPitch = false;
        wasMono = settings[mono] > .5f;
        sequencer.reset(uint32_t(settings[seed]));
        sequencer.configure(settings);
    }
    std::vector<std::vector<float>> basicFrames() const
    {
        std::vector<std::vector<float>> frames(4);
        for (int f = 0; f < 4; ++f)
            frames[size_t(f)].assign(tables.data[size_t(f)][9].begin(),
                                     tables.data[size_t(f)][9].end() - 1);
        return frames;
    }
    int active() const noexcept
    {
        int n = 0;
        for (auto &v : voices)
            n += v.envelope[0].stage != 0;
        return n;
    }
    void configurePerformance()
    {
        auto seedValue = uint32_t(settings[seed]);
        if (seedValue != configuredSeed)
        {
            configuredSeed = seedValue;
            rng = seedValue ? seedValue : 1;
            sequencer.reseed(rng);
        }
        sequencer.configure(settings);
    }
    void synchronizeTransport(bool host, bool playing, double beat, bool discontinuity)
    {
        sequencer.sync(host, playing, beat, discontinuity);
        if (discontinuity)
            globalLfo = {};
    }
    void midi(int status, int a, int b) noexcept
    {
        if (a < 0 || a > 127 || b < 0 || b > 127)
            return;
        if (!sequencer.midi(status, a, b, [this](int s, int x, int y) { rawMidi(s, x, y); }))
            rawMidi(status, a, b);
    }
    void rawMidi(int status, int a, int b) noexcept
    {
        int c = status & 15, kind = status & 240;
        if (a < 0 || a > 127 || b < 0 || b > 127)
            return;
        bool monoMode = settings[mono] > .5f;
        if (monoMode != wasMono)
        {
            reset();
            wasMono = monoMode;
        }
        if (kind == 0x90 && b)
        {
            if (monoMode)
            {
                Held *h = nullptr;
                for (auto &entry : heldNotes)
                    if (entry.note < 0)
                    {
                        h = &entry;
                        break;
                    }
                if (!h)
                {
                    h = &heldNotes[0];
                    for (auto &entry : heldNotes)
                        if (entry.age < h->age)
                            h = &entry;
                }
                *h = {a, c + 1, b, serial + 1, true, false};
                bool legatoMode =
                    voices[0].envelope[0].stage && voices[0].held && settings[legato] > .5f;
                startVoice(voices[0], c + 1, a, b, legatoMode);
            }
            else
            {
                int count = std::clamp(int(settings[polyphony]), 1, 16);
                Voice *chosen = nullptr;
                for (int i = 0; i < count; ++i)
                    if (!voices[size_t(i)].envelope[0].stage)
                    {
                        chosen = &voices[size_t(i)];
                        break;
                    }
                if (!chosen)
                    for (int i = 0; i < count; ++i)
                    {
                        auto &v = voices[size_t(i)];
                        if (!chosen ||
                            ((v.envelope[0].stage == 4) != (chosen->envelope[0].stage == 4)
                                 ? v.envelope[0].stage == 4
                                 : v.age < chosen->age))
                            chosen = &v;
                    }
                startVoice(*chosen, c + 1, a, b, false);
            }
        }
        else if (kind == 0x80 || (kind == 0x90 && !b))
        {
            if (monoMode)
            {
                Held *oldest = nullptr;
                for (auto &h : heldNotes)
                    if (h.note == a && h.channel == c + 1 && h.down &&
                        (!oldest || h.age < oldest->age))
                        oldest = &h;
                if (oldest)
                {
                    oldest->down = false;
                    oldest->sustained = pedal[size_t(c)];
                    if (!oldest->sustained)
                        oldest->note = -1;
                    monoFallback();
                }
            }
            else
            {
                Voice *chosen = nullptr;
                for (auto &v : voices)
                    if (v.envelope[0].stage && v.held && v.note == a && v.channel == c + 1 &&
                        (!chosen || v.age < chosen->age))
                        chosen = &v;
                if (chosen)
                {
                    chosen->held = false;
                    chosen->releaseVelocity = b / 127.f;
                    if (!pedal[size_t(c)] && !(settings[mpe] > .5f && pedal[0]))
                        releaseVoice(*chosen, b / 127.f);
                }
            }
        }
        else if (kind == 0xe0)
            bend[size_t(c)] = ((a + 128 * b) - 8192) / 8192.f *
                              (settings[mpe] > .5f && c > 0 ? settings[mpeRange] : 2.f);
        else if (kind == 0xd0)
            pressure[size_t(c)] = a / 127.f;
        else if (kind == 0xb0)
        {
            if (a == 1)
                wheel[size_t(c)] = b / 127.f;
            if (a == 74)
                timbreCC[size_t(c)] = b / 127.f;
            if (a == 64)
            {
                pedal[size_t(c)] = b >= 64;
                if (!pedal[size_t(c)])
                {
                    for (auto &h : heldNotes)
                        if (h.channel == c + 1 && !h.down)
                            h.note = -1;
                    if (monoMode)
                        monoFallback();
                    else
                        for (auto &v : voices)
                            if ((v.channel == c + 1 || (settings[mpe] > .5f && c == 0)) &&
                                !v.held && !pedal[size_t(v.channel - 1)])
                                releaseVoice(v, v.releaseVelocity);
                }
            }
            if (a == 120 || a == 123)
            {
                if (a == 120)
                    effects.panic();
                for (auto &h : heldNotes)
                    if (h.channel == c + 1)
                        h.note = -1;
                for (auto &v : voices)
                    if (v.channel == c + 1 || (settings[mpe] > .5f && c == 0))
                    {
                        if (a == 120)
                            v = Voice{};
                        else
                            releaseVoice(v, 0);
                    }
            }
        }
    }
    Stereo tickStereo() noexcept
    {
        assets.tick(UINT64_MAX, !externallyClockedAssets);
        bool monoMode = settings[mono] > .5f;
        if (monoMode != wasMono)
        {
            reset();
            wasMono = monoMode;
        }
        sequencer.tick(settings, rate, tempo, [this](int s, int a, int b) { rawMidi(s, a, b); });
        for (size_t i = 0; i < paramCount; ++i)
            if (parameters[i].step == 0)
                smooth[i] += alpha * (settings[i] - smooth[i]);
            else
                smooth[i] = settings[i];
        for (int l = 0; l < 10; ++l)
        {
            int base = lfo1_rate + l * 8;
            constexpr std::array<float, 9> beats{.0625f, .125f, .25f, .5f, 1, 1.5f, 2, 3, 4};
            float hz = settings[size_t(base + 2)] > .5f
                           ? float(std::max(20., tempo) / 60) /
                                 beats[size_t(std::clamp(int(settings[size_t(base + 3)]), 0, 8))]
                           : smooth[size_t(base)];
            globalLfo[size_t(l)] += hz / rate;
            globalLfo[size_t(l)] -= std::floor(globalLfo[size_t(l)]);
        }
        std::array<bool, 30> usedSources{};
        for (int r = 0; r < 16; ++r)
        {
            int b = mod1_source + r * 6;
            if (settings[size_t(b + 4)] < .5f && settings[size_t(b + 1)] > 0 &&
                std::abs(smooth[size_t(b + 2)]) > 1e-9f)
                usedSources[size_t(std::clamp(int(settings[size_t(b)]), 0, 29))] = true;
        }
        std::array<std::array<double, 7>, 3> detuneFactors{};
        std::array<std::array<float, 7>, 3> panOffsets{};
        for (int o = 0; o < 3; ++o)
        {
            auto indices = oscillatorIndices[size_t(o)];
            int count = std::clamp(int(settings[size_t(indices.unison)]), 1, 7);
            for (int u = 0; u < count; ++u)
            {
                float distance = count == 1 ? 0 : float(u) * 2 / (count - 1) - 1;
                detuneFactors[size_t(o)][size_t(u)] =
                    count == 1 ? 1 : std::exp2(distance * smooth[size_t(indices.detune)] / 1200.);
                panOffsets[size_t(o)][size_t(u)] = distance * smooth[size_t(indices.spread)];
            }
        }
        Stereo output{};
        int voiceLimit = monoMode ? 1 : std::clamp(int(settings[polyphony]), 1, 16);
        for (size_t vi = 0; vi < voices.size(); ++vi)
        {
            auto &v = voices[vi];
            if (int(vi) >= voiceLimit && v.envelope[0].stage)
                releaseVoice(v, 0);
            output = output + v.residual;
            v.residual = v.residual * residualDecay;
            if (std::abs(v.residual.l) + std::abs(v.residual.r) < 1e-9)
                v.residual = {};
            if (!v.envelope[0].stage)
                continue;
            int c = v.channel - 1;
            float bendValue = bend[size_t(c)] + (settings[mpe] > .5f && c > 0 ? bend[0] : 0),
                  pressureValue = std::clamp(pressure[size_t(c)] +
                                                 (settings[mpe] > .5f && c > 0 ? pressure[0] : 0),
                                             0.f, 1.f);
            if (v.glideLeft > 0)
            {
                v.pitch += v.pitchStep;
                if (--v.glideLeft == 0)
                    v.pitch = float(v.note);
            }
            sources = {};
            for (int e = 0; e < 4; ++e)
            {
                int base = e == 0 ? attack : env2_attack + (e - 1) * 4;
                float a = smooth[size_t(base)], d = smooth[size_t(base + 1)],
                      s = smooth[size_t(base + 2)], r = smooth[size_t(base + 3)];
                sources[size_t(e + 1)] = v.envelope[size_t(e)].tick(
                    a, d, s, r, rate, smooth[size_t(env1_attackCurve + e * 3)],
                    smooth[size_t(env1_decayCurve + e * 3)],
                    smooth[size_t(env1_releaseCurve + e * 3)]);
            }
            for (int l = 0; l < 10; ++l)
            {
                int base = lfo1_rate + l * 8;
                constexpr std::array<float, 9> beats{.0625f, .125f, .25f, .5f, 1, 1.5f, 2, 3, 4};
                float hz =
                    settings[size_t(base + 2)] > .5f
                        ? float(std::max(20., tempo) / 60) /
                              beats[size_t(std::clamp(int(settings[size_t(base + 3)]), 0, 8))]
                        : smooth[size_t(base)];
                bool retrig = settings[size_t(base + 4)] > .5f || settings[size_t(base + 6)] > .5f;
                double phase = retrig
                                   ? v.lfo[size_t(l)]
                                   : std::fmod(globalLfo[size_t(l)] + smooth[size_t(base + 5)], 1.);
                auto lfoShape = [&](double p)
                {
                    int type = int(settings[size_t(base + 1)]);
                    if (type != 5)
                        return shape(p, type);
                    double f = p * 8;
                    int a = std::clamp(int(f), 0, 7), b = (a + 1) % 8;
                    return smooth[size_t(lfo1_point1 + l * 8 + a)] +
                           float(f - a) * (smooth[size_t(lfo1_point1 + l * 8 + b)] -
                                           smooth[size_t(lfo1_point1 + l * 8 + a)]);
                };
                float value = usedSources[size_t(5 + l)]
                                  ? (v.lfoDone[size_t(l)] ? lfoShape(.999999) : lfoShape(phase))
                                  : 0;
                sources[size_t(5 + l)] =
                    usedSources[size_t(5 + l)]
                        ? (settings[size_t(base + 7)] > .5f ? (value + 1) * .5f : value)
                        : 0;
                if (retrig && !v.lfoDone[size_t(l)])
                {
                    v.lfo[size_t(l)] += hz / rate;
                    if (v.lfo[size_t(l)] >= 1)
                    {
                        if (settings[size_t(base + 6)] > .5f)
                        {
                            v.lfoDone[size_t(l)] = true;
                            v.lfo[size_t(l)] = .999999;
                        }
                        else
                            v.lfo[size_t(l)] -= std::floor(v.lfo[size_t(l)]);
                    }
                }
            }
            sources[15] = v.velocity;
            sources[16] = v.note / 127.f;
            sources[17] = v.random;
            sources[18] = wheel[size_t(c)];
            sources[19] = pressureValue;
            sources[20] = v.releaseVelocity;
            sources[21] = timbreCC[size_t(c)];
            for (int m = 0; m < 8; ++m)
                sources[size_t(22 + m)] = smooth[size_t(macro1 + m)];
            std::array<float, 22> modulation{};
            for (int r = 0; r < 16; ++r)
            {
                int base = mod1_source + r * 6;
                int src = std::clamp(int(settings[size_t(base)]), 0, 29),
                    dest = std::clamp(int(settings[size_t(base + 1)]), 0, 21);
                if (!src || !dest || settings[size_t(base + 4)] > .5f)
                    continue;
                float x = sources[size_t(src)];
                if (settings[size_t(base + 3)] > .5f && ((src >= 5 && src <= 14) || src == 17))
                    x = (x + 1) * .5f;
                if (x == 0)
                    continue;
                float curve = smooth[size_t(base + 5)];
                if (std::abs(curve) > 1e-6f)
                    x = std::copysign(std::pow(std::abs(x), std::exp2(curve * 2)), x);
                modulation[size_t(dest)] += x * smooth[size_t(base + 2)];
            }
            std::array<Stereo, 3> osc{};
            float previousOsc = 0;
            Stereo feed1{}, feed2{}, bypass{};
            for (int o = 0; o < 3; ++o)
            {
                auto idx = oscillatorIndices[size_t(o)];
                int dest = 1 + o * 5;
                float frame = std::clamp(smooth[size_t(idx.table)] + modulation[size_t(dest)] +
                                             wheel[size_t(c)] * .2f,
                                         0.f, 1.f),
                      level = std::clamp(smooth[size_t(idx.level)] + modulation[size_t(dest + 2)],
                                         0.f, 1.f),
                      pan = std::clamp(smooth[size_t(idx.pan)] + modulation[size_t(dest + 3)] * 2,
                                       -1.f, 1.f),
                      warp = std::clamp(
                          smooth[size_t(idx.warpAmount)] + modulation[size_t(dest + 4)], 0.f, 1.f);
                int count = std::clamp(int(settings[size_t(idx.unison)]), 1, 7),
                    warpType = int(settings[size_t(idx.warp)]),
                    interaction = o ? int(settings[size_t(idx.interaction)]) : 0;
                if (level < 1e-7f &&
                    (o == 2 || settings[size_t(oscillatorIndices[size_t(o + 1)].interaction)] == 0))
                {
                    previousOsc = 0;
                    continue;
                }
                double pitch = v.pitch + bendValue + smooth[size_t(idx.pitch)] +
                               smooth[size_t(idx.fine)] * .01 + modulation[size_t(dest + 1)] * 24;
                double baseFrequency = 440 * std::exp2((pitch - 69) / 12.);
                Stereo summed{};
                if (baseFrequency >= rate * .45)
                {
                    previousOsc = 0;
                    continue;
                }
                int hybridBase = hybrid1_mode + o * 18, mode = int(settings[size_t(hybridBase)]);
                if (mode > 0)
                {
                    auto generated =
                        hybridTick(v.hybrid[size_t(o)], assets.get(o), assets.revision(o), mode,
                                   hybridBase, smooth, pitch, int(v.velocity * 127), v.note, rate,
                                   [this] { return random(); });
                    generated.l *= std::sqrt(1 - pan);
                    generated.r *= std::sqrt(1 + pan);
                    previousOsc = (generated.l + generated.r) * .5f;
                    osc[size_t(o)] = generated * level;
                    int routing = int(settings[size_t(idx.route)]);
                    if (routing == 3)
                        bypass = bypass + osc[size_t(o)];
                    else
                    {
                        if (routing == 0 || routing == 2)
                            feed1 = feed1 + osc[size_t(o)];
                        if (routing == 1 || (routing == 2 && settings[filter_path] > .5f))
                            feed2 = feed2 + osc[size_t(o)];
                    }
                    continue;
                }

                int warp2Type = int(settings[size_t(osc1_warp2 + o * 2)]);
                float warp2 = smooth[size_t(osc1_warp2Amount + o * 2)];
                double safety =
                    (warp2Type ? 1 + 15 * warp2 : 1) * (warpType ? 1 + 15 * warp : 1) *
                    (interaction == 1 ? 1 + 15 * smooth[size_t(idx.interactionAmount)] : 1);
                int mip = std::clamp(
                    int(std::floor(std::log2(std::max(
                        1., rate * .45 /
                                std::max(1., baseFrequency *
                                                 detuneFactors[size_t(o)][size_t(count - 1)] *
                                                 safety))))),
                    0, Tables::levels - 1);
                for (int u = 0; u < count; ++u)
                {
                    double freq = baseFrequency * detuneFactors[size_t(o)][size_t(u)];
                    double phase = v.phase[size_t(o)][size_t(u)];
                    if (interaction == 1)
                        phase += previousOsc * smooth[size_t(idx.interactionAmount)] * .25;
                    phase -= std::floor(phase);
                    if (warpType == 1)
                    {
                        double split = .5 - .45 * warp;
                        phase = phase < split ? phase * .5 / split
                                              : .5 + (phase - split) * .5 / (1 - split);
                    }
                    else if (warpType == 2)
                        phase += std::sin(2 * pi * phase) * warp * .14;
                    else if (warpType == 3)
                        phase = std::fmod(phase * (1 + 3 * warp), 1.);
                    if (warp2Type == 1)
                    {
                        double split = .5 - .45 * warp2;
                        phase = phase < split ? phase * .5 / split
                                              : .5 + (phase - split) * .5 / (1 - split);
                    }
                    else if (warp2Type == 2)
                        phase += std::sin(2 * pi * phase) * warp2 * .14;
                    else if (warp2Type == 3)
                        phase = std::fmod(phase * (1 + 3 * warp2), 1.);
                    int l = mip;
                    auto &current = v.level[size_t(o)][size_t(u)];
                    auto &old = v.oldLevel[size_t(o)][size_t(u)];
                    auto &blend = v.blend[size_t(o)][size_t(u)];
                    if (l != current)
                    {
                        old = current;
                        current = l;
                        blend = 0;
                    }
                    auto read = [&](int mip)
                    {
                        auto *asset = assets.get(o);
                        float sample = asset && asset->kind == Asset::wavetable
                                           ? asset->read(mip, phase, frame)
                                           : tables.read(mip, phase, frame);
                        if (assets.blend(o) < 1)
                        {
                            auto *previous = assets.old(o);
                            float from = previous && previous->kind == Asset::wavetable
                                             ? previous->read(mip, phase, frame)
                                             : tables.read(mip, phase, frame);
                            sample = from * (1 - assets.blend(o)) + sample * assets.blend(o);
                        }
                        return sample;
                    };
                    float wave = read(l);
                    if (blend < 1)
                    {
                        wave = read(old) * (1 - blend) + wave * blend;
                        blend = std::min(1.f, blend + float(1 / (.002 * rate)));
                    }
                    if (interaction == 2)
                        wave *= 1 - smooth[size_t(idx.interactionAmount)] +
                                previousOsc * smooth[size_t(idx.interactionAmount)];
                    float stereoPan = std::clamp(pan + panOffsets[size_t(o)][size_t(u)], -1.f, 1.f);
                    summed.l += wave * std::sqrt(1 - stereoPan);
                    summed.r += wave * std::sqrt(1 + stereoPan);
                    v.phase[size_t(o)][size_t(u)] += freq / rate;
                    v.phase[size_t(o)][size_t(u)] -= std::floor(v.phase[size_t(o)][size_t(u)]);
                }
                previousOsc = (summed.l + summed.r) * .5f / std::sqrt(float(count));
                osc[size_t(o)] = summed * (level / std::sqrt(float(count)));
                int routing = int(settings[size_t(idx.route)]);
                if (routing == 3)
                    bypass = bypass + osc[size_t(o)];
                else
                {
                    if (routing == 0 || routing == 2)
                        feed1 = feed1 + osc[size_t(o)];
                    if (routing == 1 || (routing == 2 && settings[filter_path] > .5f))
                        feed2 = feed2 + osc[size_t(o)];
                }
            }
            double subFreq =
                440 * std::exp2((v.pitch + bendValue - 69) / 12. + settings[sub_octave]);
            int subMip =
                std::clamp(int(std::floor(std::log2(std::max(1., rate * .45 / subFreq)))), 0, 9);
            float sub = tables.read(subMip, v.subPhase, settings[sub_wave] / 3) *
                        std::clamp(smooth[sub_level] + modulation[20], 0.f, 1.f);
            v.subPhase += subFreq / rate;
            v.subPhase -= std::floor(v.subPhase);
            float noise = random();
            v.noiseState += .035f * (noise - v.noiseState);
            noise = (noise * (1 - smooth[noise_colour]) + v.noiseState * 5 * smooth[noise_colour]) *
                    std::clamp(smooth[noise_level] + modulation[21], 0.f, 1.f);
            auto panSignal = [](float x, float p)
            { return Stereo{x * std::sqrt(1 - p), x * std::sqrt(1 + p)}; };
            auto routeAux = [&](Stereo signal, int routing)
            {
                if (routing == 3)
                    bypass = bypass + signal;
                else
                {
                    if (routing == 0 || routing == 2)
                        feed1 = feed1 + signal;
                    if (routing == 1 || (routing == 2 && settings[filter_path] > .5f))
                        feed2 = feed2 + signal;
                }
            };
            routeAux(panSignal(sub, smooth[sub_pan]), int(settings[sub_route]));
            routeAux(panSignal(noise, smooth[noise_pan]), int(settings[noise_route]));
            float amp = sources[1] * v.velocity * (.8f + .2f * pressureValue);
            feed1 = feed1 * amp;
            feed2 = feed2 * amp;
            bypass = bypass * amp;
            auto filter = [&](int f, Stereo input)
            {
                float cut = f ? smooth[filter2_cutoff] : smooth[cutoff],
                      res = f ? smooth[filter2_res] : smooth[resonance],
                      tracking = f ? smooth[filter2_key] : smooth[filter1_key];
                cut *= std::exp2((v.pitch - 60) * tracking / 12);
                cut *= std::exp2(modulation[size_t(f ? 18 : 16)] * 10);
                res = std::clamp(res + modulation[size_t(f ? 19 : 17)], 0.f, 1.f);
                int type = int(settings[size_t(f ? filter2_type : filter1_type)]);
                float drive = smooth[size_t(f ? filter2_drive : filter1_drive)];
                return Stereo{v.filter[size_t(f)][0].process(input.l, cut, res, type, drive, rate),
                              v.filter[size_t(f)][1].process(input.r, cut, res, type, drive, rate)};
            };
            Stereo filtered = filter(0, feed1);
            if (settings[filter2_enabled] > .5f)
            {
                if (settings[filter_path] > .5f)
                    filtered =
                        filtered * (1 - smooth[filter_mix]) + filter(1, feed2) * smooth[filter_mix];
                else
                    filtered = filter(1, filtered + feed2);
            }
            else
                filtered = filtered + feed2;
            if (v.pendingRelease)
            {
                bool unfinished = false;
                for (int o = 0; o < 3; ++o)
                    if ((int(settings[size_t(hybrid1_mode + o * 18)]) == 1 ||
                         int(settings[size_t(hybrid1_mode + o * 18)]) == 2) &&
                        settings[size_t(hybrid1_oneShot + o)] > .5f &&
                        !v.hybrid[size_t(o)].finished &&
                        settings[size_t(oscillatorIndices[size_t(o)].level)] > 0)
                        unfinished = true;
                if (!unfinished)
                {
                    v.pendingRelease = false;
                    v.envelope[0].stop();
                }
            }
            v.last = filtered + bypass;
            output = output + v.last;
        }
        output = output * (smooth[gain] * .25f);
        if (!std::isfinite(output.l) || !std::isfinite(output.r))
        {
            reset();
            effects.panic();
            return {};
        }
        output = effects.process(output, smooth, tempo);
        if (!std::isfinite(output.l) || !std::isfinite(output.r))
        {
            reset();
            return {};
        }
        return output;
    }
    float tick() noexcept
    {
        auto s = tickStereo();
        return (s.l + s.r) * .5f;
    }
};
} // namespace prism
