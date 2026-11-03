#pragma once
#include <juce_audio_processors/juce_audio_processors.h>

// Keep the exact normalized value supplied by the host. Reconstructing it from
// a skewed native float can move by one ULP and violate AU default retention.
// APVTS still exposes native-range atomic values through RangedAudioParameter.
class PrismParameter final : public juce::RangedAudioParameter
{
    juce::NormalisableRange<float> range;
    float defaultNormalized;
    std::atomic<float> normalized;
    std::function<juce::String(float, int)> format;
    std::function<float(const juce::String &)> parse;

  public:
    PrismParameter(const juce::ParameterID &id, const juce::String &name,
                   juce::NormalisableRange<float> mapping, float initial,
                   const juce::AudioParameterFloatAttributes &attributes)
        : RangedAudioParameter(id, name, attributes.getAudioProcessorParameterWithIDAttributes()),
          range(std::move(mapping)),
          defaultNormalized(range.convertTo0to1(range.snapToLegalValue(initial))),
          normalized(defaultNormalized), format(attributes.getStringFromValueFunction()),
          parse(attributes.getValueFromStringFunction())
    {
    }
    const juce::NormalisableRange<float> &getNormalisableRange() const override
    {
        return range;
    }
    float getValue() const override
    {
        return normalized.load(std::memory_order_relaxed);
    }
    void setValue(float value) override
    {
        value = std::isfinite(value) ? juce::jlimit(0.f, 1.f, value) : defaultNormalized;
        if (range.interval > 0)
            value = range.convertTo0to1(range.snapToLegalValue(range.convertFrom0to1(value)));
        normalized.store(value, std::memory_order_relaxed);
    }
    float getDefaultValue() const override
    {
        return defaultNormalized;
    }
    int getNumSteps() const override
    {
        return AudioProcessorParameterWithID::getNumSteps();
    }
    juce::String getText(float value, int maximumLength) const override
    {
        float native = convertFrom0to1(std::isfinite(value) ? value : defaultNormalized);
        return format ? format(native, maximumLength) : juce::String(native, 3);
    }
    float getValueForText(const juce::String &text) const override
    {
        float native = parse ? parse(text) : text.getFloatValue();
        return convertTo0to1(std::isfinite(native) ? native : convertFrom0to1(defaultNormalized));
    }
};
