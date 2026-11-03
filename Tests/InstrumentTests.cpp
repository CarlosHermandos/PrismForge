#include "Engine.h"
#include "FactoryAssets.h"
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <vector>
void require(bool b, const char *message)
{
    if (!b)
    {
        std::cerr << message << '\n';
        std::exit(1);
    }
}
std::vector<prism::Stereo> render(prism::Engine &e, int samples = 12000)
{
    e.prepare(48000);
    e.midi(0x90, 60, 110);
    std::vector<prism::Stereo> result;
    result.reserve(size_t(samples));
    for (int i = 0; i < samples; ++i)
    {
        auto x = e.tickStereo();
        require(std::isfinite(x.l) && std::isfinite(x.r) && std::abs(x.l) < 8 && std::abs(x.r) < 8,
                "instrument range");
        result.push_back(x);
    }
    return result;
}
double difference(const std::vector<prism::Stereo> &a, const std::vector<prism::Stereo> &b)
{
    double sum = 0;
    for (size_t i = 0; i < a.size(); ++i)
        sum += std::abs(a[i].l - b[i].l) + std::abs(a[i].r - b[i].r);
    return sum;
}
int main()
{
    auto e = std::make_unique<prism::Engine>();
    auto original = render(*e);
    for (int o = 1; o < 3; ++o)
    {
        e->settings = prism::Settings{};
        e->settings[prism::osc1_level] = 0;
        auto idx = prism::oscillatorIndices[size_t(o)];
        e->settings[size_t(idx.level)] = 1;
        e->settings[size_t(idx.pitch)] = 12;
        auto x = render(*e);
        require(difference(x, original) > 10, "independent oscillator");
    }
    e->settings = prism::Settings{};
    e->settings[prism::osc1_unison] = 7;
    e->settings[prism::osc1_random] = .5f;
    auto stereo = render(*e);
    double separation = 0;
    for (auto x : stereo)
        separation += std::abs(x.l - x.r);
    require(separation > 1, "stereo unison");
    auto repeat = render(*e);
    require(difference(repeat, stereo) == 0, "seed reproducibility");
    for (int warp = 1; warp < 4; ++warp)
    {
        e->settings = prism::Settings{};
        e->settings[prism::osc1_warp] = float(warp);
        e->settings[prism::osc1_warpAmount] = .6f;
        require(difference(original, render(*e)) > 10, "warp audible");
    }
    for (int interaction = 1; interaction < 3; ++interaction)
    {
        e->settings = prism::Settings{};
        e->settings[prism::osc2_level] = .7f;
        e->settings[prism::osc2_interaction] = float(interaction);
        e->settings[prism::osc2_interactionAmount] = .6f;
        require(difference(original, render(*e)) > 10, "oscillator interaction");
    }
    for (int f = 0; f < 4; ++f)
    {
        e->settings = prism::Settings{};
        e->settings[prism::filter1_type] = float(f);
        e->settings[prism::cutoff] = 1200;
        e->settings[prism::resonance] = .95f;
        auto x = render(*e);
        require(difference(original, x) > 10, "filter audible");
    }
    e->settings = prism::Settings{};
    e->settings[prism::filter2_enabled] = 1;
    e->settings[prism::filter2_type] = 2;
    e->settings[prism::filter2_cutoff] = 1000;
    require(difference(original, render(*e)) > 10, "second filter");
    for (int source = 1; source <= 29; ++source)
    {
        e->settings = prism::Settings{};
        e->settings[prism::mod1_source] = float(source);
        e->settings[prism::mod1_destination] = 1;
        e->settings[prism::mod1_amount] = .4f;
        if (source >= 22)
            e->settings[size_t(prism::macro1 + source - 22)] = 1;
        e->prepare(48000);
        e->midi(0x90, 60, 110);
        e->midi(0xb0, 1, 127);
        e->midi(0xd0, 127, 0);
        e->midi(0xb0, 74, 127);
        double energy = 0;
        for (int n = 0; n < 12000; ++n)
        {
            if (n == 6000)
                e->midi(0x80, 60, 100);
            auto x = e->tickStereo();
            require(std::isfinite(x.l), "mod source finite");
            energy += x.l * x.l;
        }
        require(energy > 0, "mod source sound");
    }
    e->settings = prism::Settings{};
    e->settings[prism::mod1_source] = 15;
    e->settings[prism::mod1_destination] = 16;
    e->settings[prism::mod1_amount] = .4f;
    auto positive = render(*e);
    e->settings[prism::mod1_amount] = -.4f;
    require(difference(positive, render(*e)) > 10, "mod polarity");
    for (int sr : {44100, 48000, 96000})
    {
        for (auto &preset : prism::presets)
        {
            for (size_t i = 0; i < prism::paramCount; ++i)
                e->settings[i] = preset.values[i];
            for (int o = 0; o < 3; ++o)
            {
                int mode = int(preset.values[size_t(prism::hybrid1_mode + o * 18)]);
                e->assets.publish(
                    o, mode ? prism::makeFactoryAsset(mode, int(&preset - prism::presets.data()))
                            : nullptr);
            }
            e->prepare(sr);
            e->midi(0x90, 60, 100);
            float peak = 0;
            for (int n = 0; n < sr / 4; ++n)
            {
                auto x = e->tickStereo();
                require(std::isfinite(x.l) && std::isfinite(x.r), "preset three-rate finite");
                peak = std::max({peak, std::abs(x.l), std::abs(x.r)});
            }
            require(peak < 1, "preset clipping");
        }
    }
    for (int effect = 0; effect < 8; ++effect)
    {
        prism::Effects fx, dry;
        fx.prepare(48000);
        dry.prepare(48000);
        auto settings = prism::defaultValues(), off = settings;
        settings[size_t(prism::effectEnabled[size_t(effect)])] = 1;
        settings[prism::eq_mid] = 9;
        settings[prism::comp_threshold] = -40;
        settings[prism::limiter_ceiling] = -12;
        double delta = 0;
        for (int n = 0; n < 30000; ++n)
        {
            prism::Stereo input{float(.8 * std::sin(n * .03)), float(.6 * std::sin(n * .031))};
            auto a = fx.process(input, settings, 120), b = dry.process(input, off, 120);
            require(std::isfinite(a.l) && std::abs(a.l) < 4, "effect finite");
            delta += std::abs(a.l - b.l) + std::abs(a.r - b.r);
        }
        require(delta > 1, "effect must change audio");
    }
    e->settings = prism::Settings{};
    e->settings[prism::mono] = 1;
    e->settings[prism::glide] = .05f;
    e->prepare(48000);
    e->midi(0x90, 60, 100);
    e->midi(0x90, 64, 100);
    require(e->active() == 1, "mono voices");
    e->midi(0x80, 64, 0);
    require(e->active() == 1, "mono fallback");
    e->midi(0x80, 60, 0);
    for (int n = 0; n < 24000; ++n)
        e->tick();
    require(e->active() == 0, "mono release");
    std::cout << "Three oscillators, stereo/reproducible unison, warps/interactions, four filter "
                 "types, dual filter, 29 modulation sources, polarity, all factory programs at "
                 "three rates, "
                 "8 effects and mono fallback passed.\n";
}
