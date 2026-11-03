#include "Engine.h"
#include "FactoryAssets.h"
#include <chrono>
#include <complex>
#include <iostream>
#include <memory>
#include <vector>
int main()
{
    auto e = std::make_unique<prism::Engine>();
    std::cout
        << "scenario,sample_rate,block,voices,mean_us,peak_us,deadline_us,peak_deadline_ratio\n";
    for (int sr : {44100, 48000, 96000})
        for (int scenario = 0; scenario < 6; ++scenario)
        {
            e->settings = prism::Settings{};
            for (int o = 0; o < 3; ++o)
                e->assets.publish(o, {});
            int voices = scenario < 2 ? 16 : 8;
            const char *name = "Single oscillator";
            if (scenario == 1 || scenario == 2)
            {
                name =
                    scenario == 1 ? "Three oscillators x3 unison" : "Three oscillators x7 unison";
                voices = 16;
                for (auto indices : prism::oscillatorIndices)
                {
                    e->settings[size_t(indices.level)] = .5f;
                    e->settings[size_t(indices.unison)] = scenario == 1 ? 3.f : 7.f;
                }
            }
            if (scenario == 3 || scenario == 4)
            {
                name = scenario == 3 ? "Granular 50 per second" : "Spectral 32 harmonics";
                e->settings[prism::hybrid1_mode] = scenario == 3 ? 3.f : 4.f;
                e->settings[prism::hybrid1_grainDensity] = 50;
                e->settings[prism::hybrid1_grainSize] = .25f;
                e->assets.publish(0, prism::makeFactoryAsset(scenario == 3 ? 3 : 4, 19));
            }
            if (scenario == 5)
            {
                name = "All eight effects";
                voices = 16;
                for (auto id : prism::effectEnabled)
                    e->settings[size_t(id)] = 1;
            }
            e->prepare(sr);
            for (int n = 0; n < voices; ++n)
                e->midi(0x90, 48 + n, 90);
            for (int n = 0; n < sr / 2; ++n)
                e->tickStereo();
            double total = 0, peak = 0;
            for (int block = 0; block < 100; ++block)
            {
                auto start = std::chrono::steady_clock::now();
                for (int n = 0; n < 128; ++n)
                    e->tickStereo();
                double us = std::chrono::duration<double, std::micro>(
                                std::chrono::steady_clock::now() - start)
                                .count();
                total += us;
                peak = std::max(peak, us);
            }
            double deadline = 128000000. / sr;
            std::cout << name << ',' << sr << ",128," << voices << ',' << total / 100 << ',' << peak
                      << ',' << deadline << ',' << peak / deadline << '\n';
        }
    std::cout << "\ncoherent_table_sweep,sample_rate,frequency,frame,mip,nonharmonic_dBc\n";
    auto tables = std::make_unique<prism::Tables>();
    constexpr int N = 8192;
    for (int sr : {44100, 48000, 96000})
        for (int bin : {20, 82, 327, 1308})
            for (float frame : {0.f, 1.f / 3, 2.f / 3, 1.f})
            {
                double frequency = double(bin) * sr / N;
                int mip = std::clamp(int(std::floor(std::log2(sr * .45 / frequency))), 0, 9);
                std::vector<std::complex<float>> spectrum(N);
                for (int i = 0; i < N; ++i)
                    spectrum[size_t(i)] =
                        tables->read(mip, std::fmod(double(i) * bin / N, 1.), frame);
                prism::Asset::fft(spectrum, false);
                double total = 0, nonharmonic = 0;
                for (int k = 1; k < N / 2; ++k)
                {
                    double energy = std::norm(spectrum[size_t(k)]);
                    total += energy;
                    bool intended = k % bin == 0 && k / bin <= (1 << mip);
                    if (!intended)
                        nonharmonic += energy;
                }
                std::cout << "table," << sr << ',' << frequency << ',' << frame << ',' << mip << ','
                          << 10 * std::log10(std::max(1e-20, nonharmonic / std::max(1e-20, total)))
                          << '\n';
            }
}
