#pragma once
#include "Parameters.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
namespace prism
{
class Sequencer
{
    std::array<int, 128> held{}, physical{};
    std::array<uint64_t, 128> ages{};
    uint64_t age = 0, hash = 0;
    uint32_t rng = 1979;
    bool pedal = false, enabled = false, pattern = false, latch = false, host = false,
         playing = true, needsFlush = false, first = true;
    double beat = 0, nextStep = 0, offBeat = 0;
    int currentNote = -1, step = 0, arpIndex = 0, direction = 1, root = 60;
    float random()
    {
        rng ^= rng << 13;
        rng ^= rng >> 17;
        rng ^= rng << 5;
        return float(rng & 0xffffff) / 16777216;
    }

  public:
    void reseed(uint32_t seed)
    {
        rng = seed ? seed : 1;
    }
    void reset(uint32_t seed)
    {
        held = {};
        physical = {};
        ages = {};
        age = 0;
        hash = 0;
        rng = seed ? seed : 1;
        pedal = false;
        enabled = false;
        needsFlush = false;
        first = true;
        beat = nextStep = offBeat = 0;
        currentNote = -1;
        step = arpIndex = 0;
        direction = 1;
        root = 60;
    }
    template <class S> void configure(const S &p)
    {
        bool next = p[arp_enabled] > .5f || p[seq_enabled] > .5f;
        uint64_t h = 1469598103934665603ull;
        for (int i = arp_enabled; i <= step16_probability; ++i)
        {
            h ^= uint64_t(int64_t(p[size_t(i)] * 10000));
            h *= 1099511628211ull;
        }
        if (h != hash)
        {
            needsFlush = true;
            first = true;
            step = arpIndex = 0;
            direction = 1;
            hash = h;
        }
        enabled = next;
        pattern = p[seq_enabled] > .5f;
        latch = p[size_t(pattern ? seq_latch : arp_latch)] > .5f;
    }
    void sync(bool hasHost, bool isPlaying, double position, bool discontinuity)
    {
        host = hasHost;
        playing = isPlaying;
        if (host && std::isfinite(position))
            beat = position;
        if (discontinuity)
        {
            needsFlush = true;
            first = true;
            step = arpIndex = 0;
            direction = 1;
        }
    }
    template <class Send> bool midi(int status, int a, int b, Send send)
    {
        int kind = status & 240;
        if (!enabled)
            return false;
        if (kind == 0x90 && b)
        {
            bool empty = true;
            for (int n : physical)
                empty = empty && n == 0;
            if (empty && latch)
                held = {};
            ++physical[size_t(a)];
            ++held[size_t(a)];
            ages[size_t(a)] = ++age;
            root = a;
            return true;
        }
        if (kind == 0x80 || (kind == 0x90 && !b))
        {
            physical[size_t(a)] = std::max(0, physical[size_t(a)] - 1);
            if (!latch && !pedal)
                held[size_t(a)] = physical[size_t(a)];
            return true;
        }
        if (kind == 0xb0 && a == 64)
        {
            pedal = b >= 64;
            if (!pedal && !latch)
                held = physical;
            return true;
        }
        if (kind == 0xb0 && (a == 120 || a == 123))
        {
            held = {};
            physical = {};
            if (currentNote >= 0)
                send(0x80, currentNote, 0);
            currentNote = -1;
            first = true;
            return false;
        }
        return false;
    }
    template <class S, class Send> void tick(const S &p, double sr, double bpm, Send send)
    {
        if (needsFlush)
        {
            if (currentNote >= 0)
                send(0x80, currentNote, 0);
            currentNote = -1;
            needsFlush = false;
        }
        if (!enabled)
            return;
        std::array<int, 128> notes{};
        int count = 0;
        for (int n = 0; n < 128; ++n)
            if (held[size_t(n)] > 0)
                notes[size_t(count++)] = n;
        if ((host && !playing) || count == 0)
        {
            if (currentNote >= 0)
                send(0x80, currentNote, 0);
            currentNote = -1;
            first = true;
            if (!host)
                beat += bpm / (60 * sr);
            return;
        }
        if (currentNote >= 0 && beat + 1e-9 >= offBeat)
        {
            send(0x80, currentNote, 0);
            currentNote = -1;
        }
        constexpr std::array<double, 9> divisions{.0625, .125, .25, .5, 1, 1.5, 2, 3, 4};
        double duration = divisions[size_t(
                   std::clamp(int(p[size_t(pattern ? seq_division : arp_division)]), 0, 8))],
               swing = p[size_t(pattern ? seq_swing : arp_swing)];
        if (first)
        {
            double pair = std::floor(beat / (duration * 2)) * duration * 2,
                   split = pair + duration * (1 + swing);
            nextStep = beat <= pair + 1e-8    ? pair
                       : beat <= split + 1e-8 ? split
                                              : pair + duration * 2;
            step = int(std::floor(beat / duration)) % std::max(1, int(p[seq_length]));
            first = false;
        }
        if (beat + 1e-9 >= nextStep)
        {
            if (currentNote >= 0)
                send(0x80, currentNote, 0);
            currentNote = -1;
            int note = 0, velocity = 100;
            double gate = p[arp_gate];
            bool play = true;
            if (pattern)
            {
                int index = step % std::max(1, int(p[seq_length])), base = step1_note + index * 4;
                note = root + int(p[size_t(base)]) + int(p[seq_transpose]);
                velocity = int(p[size_t(base + 1)]);
                gate = p[size_t(base + 2)];
                play = random() < p[size_t(base + 3)] && velocity > 0 && gate > 0;
            }
            else
            {
                int order = int(p[arp_order]);
                if (order == 4)
                    std::sort(notes.begin(), notes.begin() + count,
                              [&](int a, int b) { return ages[size_t(a)] < ages[size_t(b)]; });
                int total = count * std::clamp(int(p[arp_octaves]), 1, 4), index = arpIndex;
                if (order == 1)
                    index = total - 1 - index;
                if (order == 3)
                    index = std::min(total - 1, int(random() * total));
                note = notes[size_t(index % count)] + 12 * (index / count);
                if (order == 2)
                {
                    arpIndex += direction;
                    if (arpIndex >= total)
                    {
                        arpIndex = std::max(0, total - 2);
                        direction = -1;
                    }
                    if (arpIndex < 0)
                    {
                        arpIndex = std::min(total - 1, 1);
                        direction = 1;
                    }
                }
                else
                    arpIndex = (arpIndex + 1) % total;
            }
            double stepLength = duration * (step % 2 ? 1 - swing : 1 + swing);
            if (play)
            {
                currentNote = std::clamp(note, 0, 127);
                send(0x90, currentNote, std::clamp(velocity, 1, 127));
                offBeat = nextStep + stepLength * std::clamp(gate, .001, 1.);
            }
            nextStep += stepLength;
            ++step;
        }
        beat += std::max(20., bpm) / (60 * sr);
    }
};
} // namespace prism
