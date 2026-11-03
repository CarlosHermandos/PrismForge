#pragma once
#include "Parameters.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <vector>
namespace prism
{
struct Stereo
{
    float l = 0, r = 0;
    Stereo operator+(Stereo v) const
    {
        return {l + v.l, r + v.r};
    }
    Stereo operator*(float x) const
    {
        return {l * x, r * x};
    }
};
inline float dbGain(float x)
{
    return std::pow(10.f, x * .05f);
}
class Effects
{
    struct Delay
    {
        std::vector<float> l, r;
        std::vector<uint64_t> stamps;
        uint64_t generation = 1;
        size_t pos = 0;
        void clear()
        {
            ++generation;
            pos = 0;
        }
        void prepare(int n)
        {
            l.assign(size_t(n), 0);
            r = l;
            stamps.assign(size_t(n), 0);
            generation = 1;
            pos = 0;
        }
        float read(const std::vector<float> &b, float offset) const
        {
            float p = float(pos) - std::clamp(offset, 1.f, float(b.size() - 2));
            if (p < 0)
                p += float(b.size());
            auto a = size_t(p);
            float f = p - float(a);
            size_t next = (a + 1) % b.size();
            float first = stamps[a] == generation ? b[a] : 0,
                  second = stamps[next] == generation ? b[next] : 0;
            return first + f * (second - first);
        }
        Stereo read(float n) const
        {
            return {read(l, n), read(r, n)};
        }
        void push(Stereo s)
        {
            l[pos] = s.l;
            r[pos] = s.r;
            stamps[pos] = generation;
            pos = (pos + 1) % l.size();
        }
    };
    struct Biquad
    {
        float b0 = 1, b1 = 0, b2 = 0, a1 = 0, a2 = 0, z1 = 0, z2 = 0;
        float tick(float x)
        {
            float y = b0 * x + z1;
            z1 = b1 * x - a1 * y + z2;
            z2 = b2 * x - a2 * y;
            if (std::abs(z1) < 1e-20f)
                z1 = 0;
            if (std::abs(z2) < 1e-20f)
                z2 = 0;
            return y;
        }
        void coefficients(float gain, float frequency, int type, double sr)
        {
            double A = std::pow(10., gain / 40),
                   w = 6.28318530718 * std::min(double(frequency), sr * .4) / sr, c = std::cos(w),
                   s = std::sin(w), alpha = s / (2 * .70710678), beta = 2 * std::sqrt(A) * alpha;
            double B0, B1, B2, A0, A1, A2;
            if (type == 1)
            {
                B0 = 1 + alpha * A;
                B1 = -2 * c;
                B2 = 1 - alpha * A;
                A0 = 1 + alpha / A;
                A1 = -2 * c;
                A2 = 1 - alpha / A;
            }
            else if (type == 0)
            {
                B0 = A * ((A + 1) - (A - 1) * c + beta);
                B1 = 2 * A * ((A - 1) - (A + 1) * c);
                B2 = A * ((A + 1) - (A - 1) * c - beta);
                A0 = (A + 1) + (A - 1) * c + beta;
                A1 = -2 * ((A - 1) + (A + 1) * c);
                A2 = (A + 1) + (A - 1) * c - beta;
            }
            else
            {
                B0 = A * ((A + 1) + (A - 1) * c + beta);
                B1 = -2 * A * ((A - 1) + (A + 1) * c);
                B2 = A * ((A + 1) + (A - 1) * c - beta);
                A0 = (A + 1) - (A - 1) * c + beta;
                A1 = 2 * ((A - 1) - (A + 1) * c);
                A2 = (A + 1) - (A - 1) * c - beta;
            }
            b0 = float(B0 / A0);
            b1 = float(B1 / A0);
            b2 = float(B2 / A0);
            a1 = float(A1 / A0);
            a2 = float(A2 / A0);
        }
    };
    std::array<std::array<Biquad, 3>, 2> eq;
    unsigned eqCounter = 0;
    Delay chorus, delay;
    std::array<Delay, 4> room;
    std::array<float, 8> fades{};
    std::array<bool, 8> activated{};
    std::array<float, 2> previous{}, eqLow{}, eqMid{}, compEnv{};
    std::array<std::array<float, 4>, 2> phaserState{};
    std::array<Stereo, 4> damp{};
    double sr = 48000, phaseCh = 0, phasePh = 0;
    float delaySamples = 14400;

  public:
    void panic() noexcept
    {
        chorus.clear();
        delay.clear();
        for (auto &d : room)
            d.clear();
        previous = {};
        eqLow = {};
        eqMid = {};
        compEnv = {};
        phaserState = {};
        damp = {};
        eq = {};
        eqCounter = 0;
    }
    void prepare(double rate)
    {
        sr = rate;
        chorus.prepare(int(sr * .06) + 4);
        delay.prepare(int(sr * 2.1) + 4);
        for (int i = 0; i < 4; ++i)
            room[size_t(i)].prepare(int(sr * (.12 + i * .017)) + 4);
        eq = {};
        eqCounter = 0;
        fades = {};
        activated = {};
        previous = {};
        eqLow = {};
        eqMid = {};
        compEnv = {};
        phaserState = {};
        damp = {};
        phaseCh = phasePh = 0;
        delaySamples = float(sr * .3);
    }
    template <class S> Stereo process(Stereo input, const S &p, double bpm) noexcept
    {
        std::array<bool, 8> used{};
        Stereo x = input;
        for (int slot = 0; slot < 8; ++slot)
        {
            int effect = std::clamp(int(p[fx_slot0 + slot]), 0, 7);
            if (used[size_t(effect)])
                continue;
            used[size_t(effect)] = true;
            if (p[effectEnabled[size_t(effect)]] > .5f)
                activated[size_t(effect)] = true;
            if (!activated[size_t(effect)])
                continue;
            float &fade = fades[size_t(effect)];
            fade += float(1 - std::exp(-1 / (.01 * sr))) *
                    ((p[effectEnabled[size_t(effect)]] > .5f ? 1.f : 0.f) - fade);
            Stereo wet = x;
            // Tail effects are advanced even when bypassed; their input fades to zero.
            switch (effect)
            {
            case 0:
            {
                float drive = dbGain(p[sat_drive]);
                for (int c = 0; c < 2; ++c)
                {
                    float v = c ? x.r : x.l;
                    float half = (previous[size_t(c)] + v) * .5f;
                    float y = (std::tanh(half * drive) + std::tanh(v * drive)) * .5f /
                              std::max(1.f, std::sqrt(drive));
                    previous[size_t(c)] = v;
                    if (c)
                        wet.r = y;
                    else
                        wet.l = y;
                }
                wet = x * (1 - p[sat_mix]) + wet * p[sat_mix];
                break;
            }
            case 1:
            {
                phaseCh += p[chorus_rate] / sr;
                phaseCh -= std::floor(phaseCh);
                float depth = p[chorus_depth];
                auto d = chorus.read(
                    float(sr * (.014 + .009 * depth * std::sin(6.28318530718 * phaseCh))));
                auto other = chorus.read(
                    float(sr * (.014 + .009 * depth * std::sin(6.28318530718 * (phaseCh + .25)))));
                chorus.push(x * fade);
                wet = {x.l * (1 - p[chorus_mix]) + d.l * p[chorus_mix],
                       x.r * (1 - p[chorus_mix]) + other.r * p[chorus_mix]};
                break;
            }
            case 2:
            {
                phasePh += p[phaser_rate] / sr;
                phasePh -= std::floor(phasePh);
                float f = float(500 + 1800 * p[phaser_depth] *
                                          (.5 + .5 * std::sin(6.28318530718 * phasePh)));
                float g = float(std::tan(3.14159265359 * f / sr)), a = (1 - g) / (1 + g);
                for (int c = 0; c < 2; ++c)
                {
                    float v = c ? x.r : x.l;
                    for (auto &z : phaserState[size_t(c)])
                    {
                        float y = -a * v + z;
                        z = v + a * y;
                        v = y;
                    }
                    if (c)
                        wet.r = v;
                    else
                        wet.l = v;
                }
                wet = x * (1 - p[phaser_mix]) + wet * p[phaser_mix];
                break;
            }
            case 3:
            {
                constexpr std::array<float, 9> beats{.0625f, .125f, .25f, .5f, 1, 1.5f, 2, 3, 4};
                float time = p[delay_sync] > .5f
                                 ? float(60 / std::max(20., bpm)) *
                                       beats[size_t(std::clamp(int(p[delay_division]), 0, 8))]
                                 : p[delay_time];
                delaySamples += float(1 - std::exp(-1 / (.03 * sr))) *
                                (float(std::min(2., double(time)) * sr) - delaySamples);
                auto tap = delay.read(delaySamples);
                delay.push({x.l * fade + tap.r * p[delay_feedback],
                            x.r * fade + tap.l * p[delay_feedback]});
                wet = x * (1 - p[delay_mix]) + tap * p[delay_mix];
                break;
            }
            case 4:
            {
                std::array<Stereo, 4> taps;
                Stereo sum{};
                for (int i = 0; i < 4; ++i)
                {
                    taps[size_t(i)] = room[size_t(i)].read(
                        float(sr * (.025 + i * .011) * (.6 + .8 * p[reverb_size])));
                    sum = sum + taps[size_t(i)];
                }
                for (int i = 0; i < 4; ++i)
                {
                    auto reflected = sum * .5f + taps[size_t(i)] * (-1);
                    float damping = .05f + .85f * (1 - p[reverb_damping]);
                    damp[size_t(i)] = damp[size_t(i)] * (1 - damping) + reflected * damping;
                    float feedback = .35f + .54f * p[reverb_size];
                    room[size_t(i)].push({x.l * fade * .3f + damp[size_t(i)].r * feedback,
                                          x.r * fade * .3f + damp[size_t(i)].l * feedback});
                }
                wet = x * (1 - p[reverb_mix]) + sum * (.5f * p[reverb_mix]);
                break;
            }
            case 5:
            {
                if ((eqCounter++ & 15u) == 0)
                    for (auto &channel : eq)
                    {
                        channel[0].coefficients(p[eq_low], 180, 0, sr);
                        channel[1].coefficients(p[eq_mid], p[eq_frequency], 1, sr);
                        channel[2].coefficients(p[eq_high], 6000, 2, sr);
                    }
                for (int c = 0; c < 2; ++c)
                {
                    float v = c ? x.r : x.l;
                    for (auto &band : eq[size_t(c)])
                        v = band.tick(v);
                    if (c)
                        wet.r = v;
                    else
                        wet.l = v;
                }
                break;
            }
            case 6:
            {
                float detected = std::max(std::abs(x.l), std::abs(x.r));
                float coefficient = float(std::exp(
                    -1 / ((detected > compEnv[0] ? p[comp_attack] : p[comp_release]) * .001 * sr)));
                compEnv[0] = coefficient * compEnv[0] + (1 - coefficient) * detected;
                float db = 20 * std::log10(std::max(1e-9f, compEnv[0]));
                float reduction = std::max(0.f, db - p[comp_threshold]) * (1 - 1 / p[comp_ratio]);
                wet = x * ((1 - p[comp_mix]) + p[comp_mix] * dbGain(-reduction));
                break;
            }
            case 7:
            {
                float ceiling = dbGain(p[limiter_ceiling]),
                      mag = std::max(std::abs(x.l), std::abs(x.r));
                wet = x * (mag > ceiling ? ceiling / mag : 1);
                break;
            }
            }
            x = x * (1 - fade) + wet * fade;
        }
        if (!std::isfinite(x.l) || !std::isfinite(x.r))
        {
            panic();
            return {};
        }
        return x;
    }
};
} // namespace prism
