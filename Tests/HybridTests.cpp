#include "Plugin.h"
#include <cstdlib>
#include <iostream>
void require(bool b, const char *message)
{
    if (!b)
    {
        std::cerr << message << '\n';
        std::exit(1);
    }
}
std::unique_ptr<prism::Asset> source(int mode)
{
    auto a = std::make_unique<prism::Asset>();
    a->kind = mode == 2   ? prism::Asset::multisample
              : mode == 4 ? prism::Asset::spectral
                          : prism::Asset::sample;
    prism::Asset::Zone z;
    z.l.resize(12000);
    z.r.resize(12000);
    for (int n = 0; n < 12000; ++n)
        z.l[size_t(n)] = z.r[size_t(n)] =
            float(.6 * std::sin(2 * prism::pi * 261.625565 * n / 48000));
    z.loopEnd = 12000;
    if (mode == 2)
        z.low = z.high = 60;
    a->zones.push_back(std::move(z));
    a->spectrum.resize(24);
    for (auto &frame : a->spectrum)
        frame[0] = .6f;
    a->prepareSamples();
    return a;
}
int main()
{
    juce::ScopedJuceInitialiser_GUI init;
    auto engine = std::make_unique<prism::Engine>();
    for (int mode = 1; mode <= 4; ++mode)
        for (int sr : {44100, 48000, 96000})
        {
            engine->settings = prism::Settings{};
            engine->settings[prism::hybrid1_mode] = float(mode);
            engine->settings[prism::hybrid1_loop] = 1;
            engine->settings[prism::hybrid1_grainDensity] = 100;
            engine->settings[prism::hybrid1_grainSize] = 1;
            require(engine->assets.publish(0, source(mode)), "hybrid publish");
            engine->prepare(sr);
            engine->midi(0x90, 60, 100);
            double energy = 0;
            float peak = 0;
            for (int n = 0; n < sr; ++n)
            {
                auto x = engine->tickStereo();
                require(std::isfinite(x.l) && std::isfinite(x.r), "hybrid finite");
                energy += x.l * x.l;
                peak = std::max({peak, std::abs(x.l), std::abs(x.r)});
            }
            require(energy > .01 && peak < 1, "hybrid audible level");
            engine->midi(0x80, 60, 0);
            for (int n = 0; n < sr; ++n)
                engine->tickStereo();
            require(engine->active() == 0, "hybrid release");
        }
    engine->settings = prism::Settings{};
    engine->settings[prism::hybrid1_mode] = 2;
    engine->assets.publish(0, source(2));
    engine->prepare(48000);
    engine->midi(0x90, 70, 100);
    double outside = 0;
    for (int n = 0; n < 2000; ++n)
        outside += std::abs(engine->tick());
    require(outside == 0, "multisample key range");
    auto file = juce::File("/tmp/prism-hybrid-test.wav");
    {
        juce::WavAudioFormat format;
        auto stream = file.createOutputStream();
        stream->setPosition(0);
        stream->truncate();
        std::unique_ptr<juce::OutputStream> output = std::move(stream);
        auto writer = format.createWriterFor(output, juce::AudioFormatWriterOptions()
                                                         .withSampleRate(48000)
                                                         .withNumChannels(1)
                                                         .withBitsPerSample(24));
        require(bool(writer), "test WAV writer");
        juce::AudioBuffer<float> b(1, 12000);
        for (int n = 0; n < 12000; ++n)
            b.setSample(0, n, float(.6 * std::sin(2 * prism::pi * 261.625565 * n / 48000)));
        writer->writeFromAudioSampleBuffer(b, 0, b.getNumSamples());
    }
    juce::String error;
    auto analyzed = prism::importSample(file, true, error, 60);
    require(analyzed && !analyzed->spectrum.empty(), "actual spectral analysis");
    require(analyzed->spectrum[0][0] > .4f, "harmonic magnitude");
    auto payload = prism::encodeAsset(*analyzed);
    auto decoded = prism::decodeAsset(payload, error);
    require(decoded && decoded->spectrum == analyzed->spectrum, "spectral portable state");
    auto sfz = juce::File("/tmp/prism-hybrid-test.sfz");
    sfz.replaceWithText("<group> pitch_keycenter=60\n<region> sample=prism-hybrid-test.wav "
                        "lokey=48 hikey=72 lovel=1 hivel=127 loop_mode=loop_continuous "
                        "loop_start=100 loop_end=10000 unsupported_test=1\n");
    auto multi = prism::importSFZ(sfz, error);
    require(multi && multi->zones.size() == 1 && multi->zones[0].low == 48 && multi->zones[0].loop,
            "SFZ subset");
    require(error.contains("unsupported_test"), "unsupported SFZ reporting");
    sfz.replaceWithText("<region> sample=missing.wav");
    require(!prism::importSFZ(sfz, error) && error.contains("missing"),
            "missing SFZ sample reporting");
    engine->settings = prism::Settings{};
    engine->settings[prism::mpe] = 1;
    engine->prepare(48000);
    engine->midi(0x91, 60, 100);
    engine->midi(0x92, 64, 100);
    engine->midi(0xe1, 0, 100);
    engine->midi(0xd2, 127, 0);
    engine->midi(0xb1, 74, 127);
    require(engine->active() == 2, "MPE independent voices");
    engine->midi(0xb0, 120, 0);
    require(engine->active() == 0, "MPE master panic");
    engine->settings = prism::Settings{};
    engine->settings[prism::hybrid1_mode] = 1;
    engine->settings[prism::hybrid1_oneShot] = 1;
    engine->settings[prism::release] = .001f;
    engine->assets.publish(0, source(1));
    engine->prepare(48000);
    engine->midi(0x90, 60, 100);
    for (int n = 0; n < 4800; ++n)
        engine->tick();
    engine->midi(0x80, 60, 0);
    double afterRelease = 0;
    for (int n = 0; n < 4800; ++n)
        afterRelease += std::abs(engine->tick());
    require(afterRelease > 1, "one shot ignores early release");
    for (int n = 0; n < 20000; ++n)
        engine->tick();
    require(engine->active() == 0, "one shot finishes");
    engine->settings = prism::Settings{};
    engine->settings[prism::delay_enabled] = 1;
    engine->settings[prism::delay_time] = .01f;
    engine->prepare(48000);
    engine->midi(0x90, 60, 100);
    for (int n = 0; n < 3000; ++n)
        engine->tick();
    engine->midi(0xb0, 120, 0);
    for (int n = 0; n < 3000; ++n)
        require(engine->tick() == 0, "panic clears effect tails");
    std::cout << "Sample/multisample/granular/spectral three-rate audio, bounded extreme grains, "
                 "key zones, real spectral analysis/serialization, SFZ subset/unsupported/missing "
                 "assets, MPE channels/master panic passed.\n";
}
