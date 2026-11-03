#pragma once
#include "Assets.h"
namespace prism
{
inline std::unique_ptr<Asset> makeFactoryAsset(int mode, int flavour)
{
    auto asset = std::make_unique<Asset>();
    asset->kind = mode == 2 ? Asset::multisample : mode == 4 ? Asset::spectral : Asset::sample;
    asset->root = 60;
    int zones = mode == 2 ? 8 : 1;
    for (int z = 0; z < zones; ++z)
    {
        Asset::Zone zone;
        zone.root = mode == 2 ? 36 + (z / 2) * 12 : 60;
        zone.low = mode == 2 ? (z / 2 == 0 ? 0 : 30 + (z / 2) * 12) : 0;
        zone.high = mode == 2 ? (z / 2 == 3 ? 127 : 41 + (z / 2) * 12) : 127;
        zone.lowVelocity = mode == 2 ? (z % 2 ? 81 : 0) : 0;
        zone.highVelocity = mode == 2 ? (z % 2 ? 127 : 80) : 127;
        zone.rate = 48000;
        zone.l.resize(48000);
        zone.r.resize(48000);
        zone.loopEnd = 48000;
        double fundamental = 440 * std::exp2((zone.root - 69) / 12.);
        for (int n = 0; n < 48000; ++n)
        {
            double t = n / 48000., left = 0, right = 0;
            for (int h = 1; h <= 12; ++h)
            {
                double amplitude = std::pow(double(h), -(1.2 + .1 * (flavour % 5))) *
                                   (h % 3 == flavour % 3 ? .7 : 1) * (mode == 2 && z % 2 ? .9 : 1),
                       movement =
                           .7 + .3 * std::sin(6.283185307179586 * t * (.2 + .03 * h) + h * .5);
                left += amplitude * movement * std::sin(6.283185307179586 * fundamental * h * t);
                right += amplitude * movement *
                         std::sin(6.283185307179586 * fundamental * h * t + .02 * h);
            }
            double envelope = mode == 1 || mode == 2 ? std::exp(-t * (2 + flavour % 5))
                                                     : (.6 + .4 * std::sin(3.141592653589793 * t));
            zone.l[size_t(n)] = float(left * .28 * envelope);
            zone.r[size_t(n)] = float(right * .28 * envelope);
        }
        asset->zones.push_back(std::move(zone));
    }
    if (mode == 4)
    {
        asset->spectrumHop = .01f;
        asset->spectrum.resize(100);
        for (size_t t = 0; t < asset->spectrum.size(); ++t)
            for (int h = 1; h <= 32; ++h)
                asset->spectrum[t][size_t(h - 1)] =
                    float(.25 * std::pow(double(h), -1.5) *
                          (.55 + .45 * std::sin(t * .05 + h * .5 + flavour)));
    }
    asset->prepareSamples();
    return asset;
}
} // namespace prism
