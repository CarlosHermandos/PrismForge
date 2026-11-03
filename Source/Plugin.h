#pragma once
#include "AssetIO.h"
#include "Engine.h"
#include "Parameters.h"
#include <juce_audio_utils/juce_audio_utils.h>
static_assert(std::atomic<float>::is_always_lock_free);
static_assert(std::atomic<uint64_t>::is_always_lock_free);
class PrismProcessor : public juce::AudioProcessor, private juce::MidiKeyboardState::Listener
{
  public:
    PrismProcessor();
    ~PrismProcessor() override;
    void loadHybrid(int, const juce::File &, int);
    void prepareWavetable(int, std::vector<std::vector<float>>, bool);
    void loadWavetable(int, const juce::File &, int, bool);
    std::array<std::atomic<unsigned>, 3> assetRequests{};
    juce::String getAssetStatus();
    std::atomic<unsigned> assetGeneration{0};
    juce::AudioProcessorValueTreeState state;
    prism::Engine engine;
    juce::MidiKeyboardState keyboard;
    std::array<std::atomic<float>, 512> scopeData{};
    std::atomic<unsigned> scopeIndex{0};
    unsigned scopeCursor = 0;
    std::atomic<float> peakL{0}, peakR{0};
    juce::String presetText(const juce::Identifier &);
    void setMacroNames(const std::array<juce::String, 4> &);
    std::atomic<float> peak{0};
    std::atomic<int> voiceCount{0};
    std::array<std::atomic<float> *, prism::paramCount> values{};
    void prepareToPlay(double, int) override;
    void enqueue(int status, int a, int b) noexcept;
    void releaseResources() override {}
    void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;
    bool isBusesLayoutSupported(const BusesLayout &l) const override
    {
        return l.getMainOutputChannelSet() == juce::AudioChannelSet::stereo() &&
               l.getMainInputChannelSet().isDisabled();
    }
    const juce::String getName() const override
    {
        return "Prism Forge";
    }
    bool acceptsMidi() const override
    {
        return true;
    }
    bool producesMidi() const override
    {
        return false;
    }
    double getTailLengthSeconds() const override
    {
        return 180;
    }
    bool hasEditor() const override
    {
        return true;
    }
    juce::AudioProcessorEditor *createEditor() override;
    int getNumPrograms() override
    {
        return int(prism::presets.size());
    }
    int getCurrentProgram() override
    {
        return program.load();
    }
    void setCurrentProgram(int) override;
    const juce::String getProgramName(int i) override
    {
        return prism::presets[size_t(juce::jlimit(0, int(prism::presets.size()) - 1, i))].name;
    }
    void changeProgramName(int, const juce::String &) override {}
    bool restoreState(const void *, int);
    void setPresetMetadata(const juce::String &, const juce::String &, const juce::String &,
                           const juce::String &, const juce::String &);
    void getStateInformation(juce::MemoryBlock &) override;
    void setStateInformation(const void *, int) override;

  private:
    std::atomic<int> program{0};
    juce::CriticalSection controlMutex;
    std::atomic<unsigned> stateSequence{0};
    unsigned acceptedStateSequence = 0;
    void updateSettings() noexcept;
    int64_t expectedHostSample = 0;
    bool hadHostPosition = false, lastHostPlaying = false;
    struct Event
    {
        int s, a, b;
    };
    std::array<Event, 256> queue{};
    std::atomic<unsigned> readIndex{0}, writeIndex{0};
    std::atomic<bool> overflow{false};
    void handleNoteOn(juce::MidiKeyboardState *, int c, int n, float v) override
    {
        enqueue(0x90 + c - 1, n, int(v * 127));
    }
    void handleNoteOff(juce::MidiKeyboardState *, int c, int n, float) override
    {
        enqueue(0x80 + c - 1, n, 0);
    }
    static juce::AudioProcessorValueTreeState::ParameterLayout layout();
    juce::CriticalSection assetStatusMutex;
    juce::String assetStatus;
    juce::ThreadPool assetWorkers{1};
    void assetResult(int, std::unique_ptr<prism::Asset>, const juce::String &, unsigned);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PrismProcessor)
};
