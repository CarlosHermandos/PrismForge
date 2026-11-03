#pragma once
#include "Plugin.h"
class PrismTableEditor : public juce::Component, private juce::Timer
{
    PrismProcessor &processor;
    juce::ComboBox oscillator, frameSelect, frameSize;
    juce::ToggleButton normalize{"Normalize on Apply"};
    juce::TextButton apply{"Apply to oscillator"};
    std::array<juce::TextButton, 13> buttons;
    std::array<juce::Slider, 16> harmonics;
    juce::Label status;
    std::vector<std::vector<float>> frames;
    std::vector<std::vector<std::vector<float>>> undo;
    size_t undoPosition = 0;
    int selected = 0;
    unsigned generation = 0;
    std::array<float, 16> phases{};
    bool updating = false, drawing = false, harmonicGesture = false;
    juce::Point<float> previous;
    std::unique_ptr<juce::FileChooser> chooser;
    void remember();
    void changed();
    void sync();
    void analyse();
    void importTable();
    void exportTable();
    void drawAt(juce::Point<float>);
    juce::Rectangle<float> waveformBounds() const;
    void timerCallback() override;

  public:
    explicit PrismTableEditor(PrismProcessor &);
    ~PrismTableEditor() override;
    void paint(juce::Graphics &) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent &) override;
    void mouseDrag(const juce::MouseEvent &) override;
    void mouseUp(const juce::MouseEvent &) override;
};
