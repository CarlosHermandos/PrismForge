#pragma once
#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <complex>
#include <memory>
#include <vector>
namespace prism
{
struct Asset
{
    enum Kind
    {
        wavetable,
        sample,
        multisample,
        spectral
    };
    Kind kind = wavetable;
    std::vector<std::vector<float>> frames;
    std::vector<float> tables;
    int tableSize = 2048, levels = 10;
    struct Zone
    {
        std::vector<float> l, r;
        std::vector<std::vector<float>> mipL, mipR;
        double rate = 48000;
        int root = 60, low = 0, high = 127, lowVelocity = 0, highVelocity = 127, loopStart = 0,
            loopEnd = 0;
        float tune = 0, gain = 1;
        bool loop = false;
    };
    std::vector<Zone> zones;
    std::vector<std::array<float, 32>> spectrum;
    float spectrumHop = .01f;
    int root = 60;
    float read(int level, double phase, float frame) const noexcept
    {
        if (frames.empty() || tables.empty())
            return 0;
        float f = std::clamp(frame, 0.f, 1.f) * float(frames.size() - 1);
        int a = int(f), b = std::min(int(frames.size()) - 1, a + 1);
        float position = float(phase * tableSize);
        int i = std::clamp(int(position), 0, tableSize - 1);
        float frac = position - i;
        auto lookup = [&](int w)
        {
            auto base = (size_t(w) * size_t(levels) + size_t(std::clamp(level, 0, levels - 1))) *
                        size_t(tableSize + 1);
            return tables[base + size_t(i)] +
                   frac * (tables[base + size_t(i + 1)] - tables[base + size_t(i)]);
        };
        return lookup(a) + (lookup(b) - lookup(a)) * (f - a);
    }
    bool prepareSamples()
    {
        for (auto &zone : zones)
        {
            if (zone.l.empty() || zone.r.size() != zone.l.size())
                return false;
            zone.mipL.clear();
            zone.mipR.clear();
            auto reduce = [](const std::vector<float> &input)
            {
                std::vector<float> output(input.size() / 2);
                for (size_t i = 0; i < output.size(); ++i)
                {
                    double value = 0, weights = 0;
                    for (int t = -15; t <= 16; ++t)
                    {
                        double x = t - .5,
                               window = .42 + .5 * std::cos(3.141592653589793 * x / 16) +
                                        .08 * std::cos(2 * 3.141592653589793 * x / 16),
                               weight =
                                   (std::abs(x) < 1e-9 ? .45
                                                       : std::sin(3.141592653589793 * .45 * x) /
                                                             (3.141592653589793 * x)) *
                                   window;
                        value +=
                            input[size_t(std::clamp(int(i * 2) + t, 0, int(input.size()) - 1))] *
                            weight;
                        weights += weight;
                    }
                    output[i] = float(value / weights);
                }
                return output;
            };
            for (int level = 0; level < 7; ++level)
            {
                auto &left = level ? zone.mipL.back() : zone.l;
                auto &right = level ? zone.mipR.back() : zone.r;
                if (left.size() < 64)
                    break;
                auto l = reduce(left), r = reduce(right);
                zone.mipL.push_back(std::move(l));
                zone.mipR.push_back(std::move(r));
            }
        }
        return true;
    }
    static void fft(std::vector<std::complex<float>> &a, bool inverse)
    {
        size_t n = a.size();
        for (size_t i = 1, j = 0; i < n; ++i)
        {
            size_t bit = n >> 1;
            for (; j & bit; bit >>= 1)
                j ^= bit;
            j ^= bit;
            if (i < j)
                std::swap(a[i], a[j]);
        }
        for (size_t len = 2; len <= n; len <<= 1)
        {
            float angle = float((inverse ? 2 : -2) * 3.14159265358979323846 / len);
            std::complex<float> root(std::cos(angle), std::sin(angle));
            for (size_t i = 0; i < n; i += len)
            {
                std::complex<float> w = 1;
                for (size_t j = 0; j < len / 2; ++j)
                {
                    auto u = a[i + j], v = a[i + j + len / 2] * w;
                    a[i + j] = u + v;
                    a[i + j + len / 2] = u - v;
                    w *= root;
                }
            }
        }
        if (inverse)
            for (auto &v : a)
                v /= float(n);
    }
    bool prepareTables(bool normalize)
    {
        if (frames.empty() || frames.size() > 64)
            return false;
        tables.resize(frames.size() * size_t(levels) * size_t(tableSize + 1));
        std::vector<std::complex<float>> spectrum(size_t(tableSize), 0), work;
        for (size_t f = 0; f < frames.size(); ++f)
        {
            if (frames[f].empty() || frames[f].size() > 8192)
                return false;
            std::vector<float> frame(size_t(tableSize), 0);
            double dc = 0;
            float peak = 0;
            for (int i = 0; i < tableSize; ++i)
            {
                double p = double(i) * frames[f].size() / tableSize;
                size_t index = size_t(p);
                float mix = float(p - index), v = frames[f][index] * (1 - mix) +
                                                  frames[f][(index + 1) % frames[f].size()] * mix;
                if (!std::isfinite(v))
                    return false;
                frame[size_t(i)] = v;
                dc += v / tableSize;
            }
            for (auto &v : frame)
            {
                v -= float(dc);
                peak = std::max(peak, std::abs(v));
            }
            for (auto &v : frame)
            {
                if (normalize && peak > 1e-9f)
                    v /= peak;
                v = std::clamp(v, -4.f, 4.f);
            }
            frames[f] = frame;
            for (int i = 0; i < tableSize; ++i)
                spectrum[size_t(i)] = frame[size_t(i)];
            fft(spectrum, false);
            for (int l = 0; l < levels; ++l)
            {
                work = spectrum;
                int harmonic = 1 << l;
                for (int k = harmonic + 1; k < tableSize - harmonic; ++k)
                    work[size_t(k)] = 0;
                fft(work, true);
                auto base = (f * size_t(levels) + size_t(l)) * size_t(tableSize + 1);
                for (int i = 0; i < tableSize; ++i)
                    tables[base + size_t(i)] = work[size_t(i)].real();
                tables[base + size_t(tableSize)] = tables[base];
            }
        }
        return true;
    }
};
// Ownership remains on the publishing/control thread. The audio thread only changes
// bounded slot states and reads immutable payloads; it never deletes an asset.
class AssetPool
{
    enum State
    {
        free,
        writing,
        ready,
        active,
        retired
    };
    struct Slot
    {
        std::unique_ptr<Asset> asset;
        std::atomic<int> state{free}, pins{0};
    };
    std::array<Slot, 9> slots;
    std::array<std::atomic<int>, 3> current{{-1, -1, -1}};
    std::array<std::atomic<uint64_t>, 3> pending{}, pendingVersion{};
    uint64_t serial = 0;
    std::array<int, 3> previous{{-1, -1, -1}}, fade{};
    std::array<unsigned, 3> version{};

  public:
    struct Pin
    {
        AssetPool *pool = nullptr;
        int slot = -1;
        const Asset *asset = nullptr;
        Pin() = default;
        Pin(AssetPool *p, int s, const Asset *a) : pool(p), slot(s), asset(a) {}
        Pin(const Pin &) = delete;
        Pin(Pin &&other) noexcept : pool(other.pool), slot(other.slot), asset(other.asset)
        {
            other.pool = nullptr;
        }
        ~Pin()
        {
            if (pool)
                pool->slots[size_t(slot)].pins.fetch_sub(1);
        }
    };
    bool publish(int oscillator, std::unique_ptr<Asset> asset, uint64_t version = 0)
    {
        if (oscillator < 0 || oscillator > 2)
            return false;
        auto stale = pending[size_t(oscillator)].exchange(0);
        if (stale)
            slots[size_t((stale & 15) - 1)].state.store(free);
        for (int i = oscillator * 3; i < oscillator * 3 + 3; ++i)
        {
            int expected = free;
            if (slots[size_t(i)].pins.load() == 0 &&
                slots[size_t(i)].state.compare_exchange_strong(expected, writing))
            {
                slots[size_t(i)].asset = std::move(asset);
                slots[size_t(i)].state.store(ready, std::memory_order_release);
                pendingVersion[size_t(oscillator)].store(version, std::memory_order_release);
                pending[size_t(oscillator)].store((++serial << 4) | uint64_t(i + 1),
                                                  std::memory_order_release);
                return true;
            }
        }
        return false;
    }
    void tick(uint64_t accepted = UINT64_MAX, bool commit = true, bool advance = true)
    {
        for (size_t o = 0; o < 3; ++o)
        {
            int index = -1;
            auto token = pending[o].load(std::memory_order_acquire);
            if (commit && token && pendingVersion[o].load(std::memory_order_acquire) <= accepted &&
                pending[o].compare_exchange_strong(token, 0, std::memory_order_acq_rel))
                index = int((token & 15) - 1);
            if (index >= 0)
            {
                ++version[o];
                if (previous[o] >= 0)
                    slots[size_t(previous[o])].state.store(free, std::memory_order_release);
                previous[o] = current[o].load();
                slots[size_t(index)].state.store(active, std::memory_order_release);
                current[o].store(index, std::memory_order_release);
                if (previous[o] >= 0)
                    slots[size_t(previous[o])].state.store(retired, std::memory_order_release);
                fade[o] = 128;
            }
            if (advance && fade[o] > 0 && --fade[o] == 0 && previous[o] >= 0)
            {
                slots[size_t(previous[o])].state.store(free, std::memory_order_release);
                previous[o] = -1;
            }
        }
    }
    unsigned revision(int o) const noexcept
    {
        return version[size_t(o)];
    }
    const Asset *get(int o) const noexcept
    {
        int i = current[size_t(o)].load(std::memory_order_relaxed);
        return i >= 0 ? slots[size_t(i)].asset.get() : nullptr;
    }
    const Asset *old(int o) const noexcept
    {
        int i = previous[size_t(o)];
        return i >= 0 ? slots[size_t(i)].asset.get() : nullptr;
    }
    float blend(int o) const noexcept
    {
        return 1 - fade[size_t(o)] / 128.f;
    }
    Pin pin(int o)
    {
        auto token = pending[size_t(o)].load();
        int i = token ? int((token & 15) - 1) : current[size_t(o)].load();
        if (i < 0)
            return {};
        auto &slot = slots[size_t(i)];
        slot.pins.fetch_add(1);
        int s = slot.state.load();
        if (s == active || s == retired || s == ready)
            return {this, i, slot.asset.get()};
        slot.pins.fetch_sub(1);
        return {};
    }
};
} // namespace prism
