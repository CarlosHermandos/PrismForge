#pragma once
#include "Assets.h"
#include "Effects.h"
namespace prism
{
struct HybridVoice
{
    struct Grain
    {
        double position = 0, step = 1;
        int age = 0, length = 1;
        float pan = 0, leftGain = 1, rightGain = 1;
        double windowCos = 1, windowSin = 0, rotateCos = 1, rotateSin = 0;
        bool active = false;
    };
    std::array<Grain, 64> grains{};
    double position = 0, spectralPhase = 0, grainClock = 0, motion = 0;
    unsigned revision = 0;
    int mode = -1, zone = -1;
    float fade = 1;
    Stereo last{}, tail{};
    bool finished = false;
};
inline float cubic(const std::vector<float> &data, double position)
{
    if (data.empty())
        return 0;
    int i = int(std::floor(position));
    float f = float(position - i);
    auto at = [&](int p) { return data[size_t(std::clamp(p, 0, int(data.size()) - 1))]; };
    float a = at(i - 1), b = at(i), c = at(i + 1), d = at(i + 2);
    return b + .5f * f * (c - a + f * (2 * a - 5 * b + 4 * c - d + f * (3 * (b - c) + d - a)));
}
inline Stereo sampleRead(const Asset::Zone &zone, double position, double step)
{
    if (std::abs(step) > std::exp2(double(zone.mipL.size() + 1)))
        return {};
    double level =
        std::clamp(std::log2(std::max(1., std::abs(step))), 0., double(zone.mipL.size()));
    int a = int(level), b = std::min(int(zone.mipL.size()), a + 1);
    auto read = [&](int mip)
    {
        if (!mip)
            return Stereo{cubic(zone.l, position), cubic(zone.r, position)};
        double p = position / std::exp2(mip);
        return Stereo{cubic(zone.mipL[size_t(mip - 1)], p), cubic(zone.mipR[size_t(mip - 1)], p)};
    };
    return read(a) * (1 - float(level - a)) + read(b) * float(level - a);
}
template <class S, class Random>
Stereo hybridTick(HybridVoice &state, const Asset *asset, unsigned revision, int mode, int base,
                  const S &p, double note, int velocity, int key, double sr, Random random)
{
    if (!asset || asset->zones.empty())
        return {};
    if (state.revision != revision || state.mode != mode)
    {
        state.tail = state.last;
        state.fade = 0;
        state.revision = revision;
        state.mode = mode;
        state.finished = false;
        state.zone = -1;
        state.grains = {};
        state.grainClock = 0;
        state.motion = 0;
        for (size_t z = 0; z < asset->zones.size(); ++z)
        {
            auto &zone = asset->zones[z];
            if (mode != 2 || (key >= zone.low && key <= zone.high && velocity >= zone.lowVelocity &&
                              velocity <= zone.highVelocity))
            {
                state.zone = int(z);
                break;
            }
        }
        if (state.zone < 0)
            return {};
        auto &zone = asset->zones[size_t(state.zone)];
        double start = std::min(double(p[size_t(base + 2)]), double(p[size_t(base + 3)])) *
                       (zone.l.size() - 1),
               end = std::max(double(p[size_t(base + 2)]), double(p[size_t(base + 3)])) *
                     (zone.l.size() - 1);
        state.position = p[size_t(base + 4)] > .5f ? end : start;
    }
    if (state.zone < 0 || state.zone >= int(asset->zones.size()))
        return {};
    auto &zone = asset->zones[size_t(state.zone)];
    double root = mode == 2 ? zone.root : p[size_t(base + 1)],
           step = zone.rate / sr * std::exp2((note - root + zone.tune * .01) / 12);
    double start = std::min(double(p[size_t(base + 2)]), double(p[size_t(base + 3)])) *
                   (zone.l.size() - 1),
           end = std::max(double(p[size_t(base + 2)]), double(p[size_t(base + 3)])) *
                 (zone.l.size() - 1);
    if (end - start < 4)
        return {};
    Stereo output{};
    if (mode == 1 || mode == 2)
    {
        bool looping = p[size_t(base + 5)] > .5f ||
                       (zone.loop && p[size_t(hybrid1_oneShot + (base - hybrid1_mode) / 18)] < .5f);
        double low = zone.loop ? zone.loopStart : start, high = zone.loop ? zone.loopEnd - 1 : end;
        low = std::clamp(low, start, end - 3);
        high = std::clamp(high, low + 3, end);
        double direction = p[size_t(base + 4)] > .5f ? -1 : 1;
        if (!state.finished)
        {
            output = sampleRead(zone, state.position, step);
            double crossfade =
                std::min(double(p[size_t(base + 6)]) * zone.rate, (high - low) * .49);
            if (looping && crossfade > 1)
            {
                if (direction > 0 && state.position > high - crossfade)
                {
                    float blend = float((state.position - (high - crossfade)) / crossfade);
                    output =
                        output * (1 - blend) +
                        sampleRead(zone, low + state.position - (high - crossfade), step) * blend;
                }
                if (direction < 0 && state.position < low + crossfade)
                {
                    float blend = float((low + crossfade - state.position) / crossfade);
                    output =
                        output * (1 - blend) +
                        sampleRead(zone, high - (low + crossfade - state.position), step) * blend;
                }
            }
            state.position += direction * step;
            if (looping)
            {
                double length = std::max(1., high - low - crossfade);
                if (state.position > high)
                    state.position = low + crossfade + std::fmod(state.position - high, length);
                if (state.position < low)
                    state.position = high - crossfade - std::fmod(low - state.position, length);
            }
            else if (state.position > end || state.position < start)
                state.finished = true;
        }
    }
    else if (mode == 3)
    {
        state.motion += double(p[size_t(base + 14)]) * zone.rate / (sr * zone.l.size());
        state.motion -= std::floor(state.motion);
        state.grainClock -= 1;
        if (state.grainClock <= 0)
        {
            state.grainClock += sr / std::max(1.f, p[size_t(base + 10)]);
            for (auto &grain : state.grains)
                if (!grain.active)
                {
                    double normalized = std::fmod(p[size_t(base + 8)] + state.motion +
                                                      random() * p[size_t(base + 13)] + 2,
                                                  1.);
                    grain.position = start + normalized * (end - start);
                    grain.step = step * std::exp2(p[size_t(base + 11)] / 12);
                    grain.length = std::max(8, int(p[size_t(base + 9)] * sr));
                    grain.age = 0;
                    grain.pan = random() * p[size_t(base + 12)];
                    grain.leftGain = std::sqrt(1 - grain.pan);
                    grain.rightGain = std::sqrt(1 + grain.pan);
                    grain.windowCos = 1;
                    grain.windowSin = 0;
                    grain.rotateCos = std::cos(6.283185307179586 / grain.length);
                    grain.rotateSin = std::sin(6.283185307179586 / grain.length);
                    grain.active = true;
                    break;
                }
        }
        float weightSum = 0;
        for (auto &grain : state.grains)
            if (grain.active)
            {
                float window = float(.5 - .5 * grain.windowCos);
                double nextSin =
                    grain.windowSin * grain.rotateCos + grain.windowCos * grain.rotateSin;
                grain.windowCos =
                    grain.windowCos * grain.rotateCos - grain.windowSin * grain.rotateSin;
                grain.windowSin = nextSin;
                auto sample = sampleRead(zone, grain.position, grain.step);
                output.l += sample.l * window * grain.leftGain;
                output.r += sample.r * window * grain.rightGain;
                weightSum += window;
                grain.position += grain.step;
                double length = end - start;
                while (grain.position > end)
                    grain.position -= length;
                if (++grain.age >= grain.length)
                    grain.active = false;
            }
        output = output * (1 / std::max(1.f, weightSum));
    }
    else if (mode == 4 && !asset->spectrum.empty())
    {
        float position = p[size_t(base + 15)] * float(asset->spectrum.size() - 1);
        int a = int(position), b = std::min(int(asset->spectrum.size()) - 1, a + 1),
            count = std::clamp(int(p[size_t(base + 17)]), 1, 32);
        double freq = 440 * std::exp2((note - 69) / 12), value = 0, total = 0;
        double angle = state.spectralPhase * 6.283185307179586, sine = std::sin(angle),
               cosine = std::cos(angle), sinH = sine, cosH = cosine;
        for (int h = 1; h <= count; ++h)
        {
            if (freq * h >= sr * .45)
                break;
            float amplitude = asset->spectrum[size_t(a)][size_t(h - 1)] * (1 - (position - a)) +
                              asset->spectrum[size_t(b)][size_t(h - 1)] * (position - a);
            amplitude *= std::pow(float(h), -p[size_t(base + 16)] * 2);
            value += amplitude * sinH;
            total += amplitude;
            double nextSin = sinH * cosine + cosH * sine;
            cosH = cosH * cosine - sinH * sine;
            sinH = nextSin;
        }
        output.l = output.r = float(value / std::max(1., total));
        state.spectralPhase += freq / sr;
        state.spectralPhase -= std::floor(state.spectralPhase);
    }
    output = output * (p[size_t(base + 7)] * zone.gain);
    if (state.fade < 1)
    {
        output = state.tail * (1 - state.fade) + output * state.fade;
        state.fade = std::min(1.f, state.fade + 1 / 128.f);
    }
    state.last = output;
    return output;
}
} // namespace prism
