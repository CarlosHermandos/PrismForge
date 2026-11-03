#pragma once
#include "Assets.h"
#include <juce_audio_formats/juce_audio_formats.h>
namespace prism
{
juce::String encodeAsset(const Asset &);
std::unique_ptr<Asset> decodeAsset(const juce::String &, juce::String &);
std::unique_ptr<Asset> importSample(const juce::File &, bool, juce::String &, int root = 60);
std::unique_ptr<Asset> importSFZ(const juce::File &, juce::String &);
std::unique_ptr<Asset> importWavetable(const juce::File &, int, bool, juce::String &);
} // namespace prism
