#pragma once
#include "Plugin.h"
class PrismScope : public juce::Component
{
    PrismProcessor &processor;

  public:
    explicit PrismScope(PrismProcessor &p) : processor(p) {}
    void paint(juce::Graphics &g) override
    {
        g.fillAll(juce::Colour(0xff1b242b));
        auto width = float(getWidth()), height = float(getHeight());
        g.setColour(juce::Colour(0xff9cb2c0));
        g.setFont(juce::FontOptions(12));
        g.drawText("OUTPUT WAVEFORM / LAST 512 SAMPLES", 18, 10, getWidth() - 36, 25,
                   juce::Justification::centredLeft);
        g.drawText("WINDOWED FFT / 512 SAMPLES / DISPLAY ONLY", 18, int(height * .52f),
                   getWidth() - 36, 25, juce::Justification::centredLeft);
        unsigned head = processor.scopeIndex.load(std::memory_order_acquire);
        std::vector<std::complex<float>> bins(512);
        juce::Path waveform;
        float centre = height * .27f, scale = height * .18f;
        for (unsigned i = 0; i < 512; ++i)
        {
            float sample = processor.scopeData[(head + i) % 512].load(std::memory_order_relaxed);
            bins[i] = sample * float(.5 - .5 * std::cos(2 * prism::pi * i / 511));
            float x = 18 + i * (width - 36) / 511,
                  y = centre - std::clamp(sample, -1.f, 1.f) * scale;
            if (!i)
                waveform.startNewSubPath(x, y);
            else
                waveform.lineTo(x, y);
        }
        g.setColour(juce::Colour(0xff34414a));
        g.drawHorizontalLine(int(centre), 18, width - 18);
        g.setColour(juce::Colour(0xff61b6ec));
        g.strokePath(waveform, juce::PathStrokeType(1.4f));
        prism::Asset::fft(bins, false);
        float top = height * .61f, bottom = height - 25;
        juce::Path spectrum;
        double sr = processor.getSampleRate() > 0 ? processor.getSampleRate() : 48000;
        for (int i = 1; i < 256; ++i)
        {
            float db = 20 * std::log10(std::max(1e-7f, std::abs(bins[size_t(i)]) / 128));
            float x = 18 + float(std::log(double(i)) / std::log(255.)) * (width - 36),
                  y = bottom - juce::jlimit(0.f, 1.f, (db + 90) / 90) * (bottom - top);
            if (i == 1)
                spectrum.startNewSubPath(x, y);
            else
                spectrum.lineTo(x, y);
        }
        g.setColour(juce::Colour(0xffb4dc6b));
        g.strokePath(spectrum, juce::PathStrokeType(1.4f));
        g.setColour(juce::Colour(0xff9cb2c0));
        g.drawText(juce::String(sr / 512, 1) + " Hz bins / log frequency / -90 to 0 dBFS", 18,
                   getHeight() - 23, getWidth() - 36, 20, juce::Justification::centredLeft);
    }
};
