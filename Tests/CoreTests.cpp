#include <memory>

#include "Engine.h"
#include "FactoryAssets.h"
#include "Parameters.h"
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>
void check(bool b, const char *msg)
{
    if (!b)
    {
        std::cerr << msg << '\n';
        std::exit(1);
    }
}
void wav(const std::string &path, const std::vector<float> &data, int sr)
{
    std::ofstream f(path, std::ios::binary);
    auto u16 = [&](unsigned n)
    {
        f.put(char(n));
        f.put(char(n >> 8));
    };
    auto u32 = [&](unsigned n)
    {
        u16(n & 65535);
        u16(n >> 16);
    };
    f.write("RIFF", 4);
    u32(36 + unsigned(data.size() * 2));
    f.write("WAVEfmt ", 8);
    u32(16);
    u16(1);
    u16(1);
    u32(sr);
    u32(sr * 2);
    u16(2);
    u16(16);
    f.write("data", 4);
    u32(unsigned(data.size() * 2));
    for (float x : data)
        u16(unsigned(int16_t(std::clamp(x, -1.f, 1.f) * 32767)));
}
int main(int argc, char **argv)
{
    auto e = std::make_unique<prism::Engine>();
    for (int sr : {44100, 48000, 96000})
    {
        e->settings = {0, .001f, .02f, 1, .05f, 20000, 0, .8f};
        e->prepare(sr);
        e->midi(0x90, 69, 127);
        int crosses = 0;
        float prev = 0;
        double first = 0, last = 0;
        float peak = 0;
        for (int n = 0; n < sr; ++n)
        {
            float x = e->tick();
            check(std::isfinite(x), "nonfinite");
            peak = std::max(peak, std::abs(x));
            if (prev < 0 && x >= 0 && n > sr / 10)
            {
                double t = n - x / (x - prev);
                if (crosses == 0)
                    first = t;
                last = t;
                ++crosses;
            }
            prev = x;
        }
        double hz = (crosses - 1) * sr / (last - first);
        check(std::abs(1200 * std::log2(hz / 440)) < 1, "pitch >1 cent");
        check(peak < 1 && peak > .01f, "level");
        e->midi(0x80, 69, 0);
        for (int n = 0; n < sr / 10; ++n)
            e->tick();
        check(!e->active(), "release");
        e->midi(0xb0, 64, 127);
        e->midi(0x90, 60, 100);
        e->midi(0x80, 60, 0);
        for (int i = 0; i < sr / 5; ++i)
            e->tick();
        check(e->active() == 1, "sustain hold");
        e->midi(0xb0, 64, 0);
        for (int i = 0; i < sr / 5; ++i)
            e->tick();
        check(!e->active(), "sustain release");
        e->midi(0x90, 60, 100);
        e->midi(0x90, 60, 100);
        e->midi(0x80, 60, 0);
        for (int i = 0; i < sr / 5; ++i)
            e->tick();
        check(e->active() == 1, "overlap FIFO");
        e->midi(0xb0, 120, 0);
        check(!e->active(), "panic");
        for (int n = 0; n < 32; ++n)
        {
            e->midi(0x90, 40 + n, 127);
            for (int i = 0; i < 17; ++i)
                check(std::isfinite(e->tick()), "steal finite");
        }
        check(e->active() == 16, "polyphony");
        e->settings.cutoff = 20000;
        e->settings.resonance = 1;
        for (int i = 0; i < sr; ++i)
            check(std::isfinite(e->tick()) && std::abs(e->tick()) < 8, "filter runaway");
        std::cout << "rate=" << sr << " pitch=" << hz << " peak=" << peak
                  << " MIDI/envelope/filter passed\n";
    }
    e->settings = {0, .001f, .01f, 1, .01f, 20000, 0, .8f};
    e->prepare(48000);
    e->midi(0xe0, 127, 127);
    e->midi(0x90, 69, 127);
    int bentCrossings = 0;
    float previous = 0;
    for (int i = 0; i < 48000; ++i)
    {
        float x = e->tick();
        if (previous < 0 && x >= 0)
            ++bentCrossings;
        previous = x;
    }
    check(bentCrossings >= 492 && bentCrossings <= 495, "pitch bend range");
    e->midi(0xd0, 127, 0);
    e->midi(0xb0, 1, 127);
    for (int i = 0; i < 1000; ++i)
        check(std::isfinite(e->tick()), "performance controllers finite");
    for (size_t p = 0; p < prism::presets.size(); ++p)
    {
        auto v = prism::presets[p].values;
        e->settings = {v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7]};
        for (size_t j = 0; j < prism::paramCount; ++j)
            e->settings[j] = v[j];
        for (int o = 0; o < 3; ++o)
        {
            int mode = int(v[size_t(prism::hybrid1_mode + o * 18)]);
            e->assets.publish(o, mode ? prism::makeFactoryAsset(mode, int(p)) : nullptr);
        }
        e->prepare(48000);
        std::vector<float> samples(48000 * 5);
        float peak = 0;
        double energy = 0;
        for (size_t n = 0; n < samples.size(); ++n)
        {
            if (n == 0)
            {
                e->midi(0x90, std::string_view(prism::presets[p].category) == "Bass" ? 36 : 60,
                        100);
                e->midi(0xb0, 1, 32);
            }
            if (n == 96000)
                e->midi(0xb0, 123, 0);
            samples[n] = e->tick();
            check(std::isfinite(samples[n]), "preset finite");
            peak = std::max(peak, std::abs(samples[n]));
            energy += samples[n] * samples[n];
        }
        check(peak > .001 && peak < 1, "preset peak");
        check(!e->active(), "preset stuck");
        if (argc > 1)
            wav(std::string(argv[1]) + "/" + std::to_string(p) + ".wav", samples, 48000);
        std::cout << prism::presets[p].name << " peak=" << peak
                  << " rms=" << std::sqrt(energy / samples.size()) << '\n';
    }
    // EDM patches must also tolerate real multi-note playing and ordinary note-offs.
    for (size_t index = 65; index < prism::presets.size(); ++index)
    {
        const auto &patch = prism::presets[index];
        for (size_t j = 0; j < prism::paramCount; ++j)
        {
            const auto &d = prism::parameters[j];
            check(std::isfinite(patch.values[j]) && patch.values[j] >= d.min &&
                      patch.values[j] <= d.max,
                  "EDM parameter outside range");
            e->settings[j] = patch.values[j];
        }
        e->prepare(48000);
        const int root = std::string_view(patch.category) == "Bass" ? 36 : 60;
        for (int interval : {0, 4, 7, 12})
            e->midi(0x90, root + interval, 110);
        float peak = 0;
        const int hold = 24000;
        const int length = hold + int(48000 * std::max(1.f, patch.values[prism::release] * 2));
        for (int n = 0; n < length; ++n)
        {
            if (n == hold)
                for (int interval : {0, 4, 7, 12})
                    e->midi(0x80, root + interval, 0);
            auto x = e->tickStereo();
            check(std::isfinite(x.l) && std::isfinite(x.r), "EDM chord finite");
            peak = std::max({peak, std::abs(x.l), std::abs(x.r)});
        }
        check(peak > .001f && peak < 1.f, "EDM chord output level");
        check(e->active() == 0, "EDM ordinary note-off stuck");
        std::cout << patch.name << " four-note peak=" << peak << '\n';
    }
    // Coherent high-note table residual: energy outside the intended fundamental.
    {
        auto tables = std::make_unique<prism::Tables>();
        constexpr int N = 4096, bin = 853;
        double energy = 0, re = 0, im = 0;
        for (int i = 0; i < N; ++i)
        {
            double phase = std::fmod(double(i) * bin / N, 1.0);
            double x = tables->read(0, phase, 2.f / 3);
            energy += x * x;
            re += x * std::cos(2 * prism::pi * bin * i / N);
            im += x * std::sin(2 * prism::pi * bin * i / N);
        }
        double residual = std::max(1e-16, (energy - 2 * (re * re + im * im) / N) / energy);
        double db = 10 * std::log10(residual);
        check(db < -55, "table interpolation spectral residual");
        std::cout << "48k table saw at 9996.09 Hz, 1 harmonic: residual dBc=" << db << '\n';
    }
    e->prepare(48000);
    for (int n = 48; n < 64; ++n)
        e->midi(0x90, n, 100);
    double maxUs = 0, totalUs = 0;
    for (int b = 0; b < 1000; ++b)
    {
        auto start = std::chrono::steady_clock::now();
        for (int i = 0; i < 128; ++i)
            e->tick();
        auto us =
            std::chrono::duration<double, std::micro>(std::chrono::steady_clock::now() - start)
                .count();
        maxUs = std::max(maxUs, us);
        totalUs += us;
    }
    std::cout << "16 voices / 48k / 128 samples: mean_us=" << totalUs / 1000 << " peak_us=" << maxUs
              << '\n';
}
