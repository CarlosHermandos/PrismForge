#include "Plugin.h"
#include <cstdlib>
#include <iostream>
void require(bool b, const char *msg)
{
    if (!b)
    {
        std::cerr << msg << '\n';
        std::exit(1);
    }
}
int main()
{
    juce::ScopedJuceInitialiser_GUI init;
    auto asset = std::make_unique<prism::Asset>();
    asset->frames.assign(3, std::vector<float>(2048));
    for (int f = 0; f < 3; ++f)
        for (int n = 0; n < 2048; ++n)
            asset->frames[size_t(f)][size_t(n)] =
                float(std::sin(2 * prism::pi * (f + 1) * n / 2048));
    require(asset->prepareTables(true), "table generation");
    auto encoded = prism::encodeAsset(*asset);
    juce::String error;
    auto decoded = prism::decodeAsset(encoded, error);
    require(decoded && decoded->frames == asset->frames, "asset codec");
    require(!prism::decodeAsset("bad payload", error), "malformed codec");
    require(
        !prism::importWavetable(juce::File("/tmp/nonexistent-prism-asset.wav"), 2048, true, error),
        "missing file");
    auto p = std::make_unique<PrismProcessor>();
    require(p->engine.assets.publish(0, std::move(asset)), "publish");
    juce::MemoryBlock saved;
    p->getStateInformation(saved);
    auto q = std::make_unique<PrismProcessor>();
    require(q->restoreState(saved.getData(), int(saved.getSize())), "asset state restore");
    auto pin = q->engine.assets.pin(0);
    require(pin.asset && pin.asset->frames == decoded->frames, "embedded portable asset");
    p->prepareToPlay(48000, 128);
    q->prepareToPlay(48000, 128);
    juce::AudioBuffer<float> a(2, 128), b(2, 128);
    juce::MidiBuffer m, n;
    m.addEvent(juce::MidiMessage::noteOn(1, 60, 1.f), 0);
    n = m;
    for (int block = 0; block < 30; ++block)
    {
        p->processBlock(a, m);
        q->processBlock(b, n);
        for (int i = 0; i < 128; ++i)
            require(a.getSample(0, i) == b.getSample(0, i), "custom table recall audio");
    }
    for (int replacement = 0; replacement < 20; ++replacement)
    {
        auto table = std::make_unique<prism::Asset>();
        table->frames = decoded->frames;
        require(table->prepareTables(false), "replacement prepare");
        require(p->engine.assets.publish(0, std::move(table)), "replacement slot");
        for (int i = 0; i < 256; ++i)
        {
            auto sample = p->engine.tickStereo();
            require(std::isfinite(sample.l), "replacement render");
        }
    }
    std::cout << "FFT mip construction, payload round trip/rejection, missing file, embedded "
                 "asset/session audio equivalence and 20 live replacements passed.\n";
}
