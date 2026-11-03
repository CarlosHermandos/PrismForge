#pragma once
#include "Plugin.h"
#include "PresetBrowser.h"
#include "Scope.h"
#include "WavetableEditor.h"
class PrismLookAndFeel : public juce::LookAndFeel_V4
{
  public:
    PrismLookAndFeel();
    juce::Font getComboBoxFont(juce::ComboBox &) override
    {
        return juce::Font(juce::FontOptions(11));
    }
    juce::Font getTextButtonFont(juce::TextButton &, int) override
    {
        return juce::Font(juce::FontOptions(11));
    }
    void drawRotarySlider(juce::Graphics &, int, int, int, int, float, float, float,
                          juce::Slider &) override;
    void drawButtonBackground(juce::Graphics &, juce::Button &, const juce::Colour &, bool,
                              bool) override;
};
class PrismControl : public juce::Component, public juce::DragAndDropTarget
{
    PrismProcessor &processor;
    size_t parameter;
    juce::Label label;
    juce::Slider slider;
    juce::ComboBox choice;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> choiceAttachment;

  public:
    PrismControl(PrismProcessor &, size_t);
    void resized() override;
    void refreshEnabled();
    void paintOverChildren(juce::Graphics &) override;
    bool isInterestedInDragSource(const SourceDetails &) override;
    void itemDropped(const SourceDetails &) override;
    void mouseDrag(const juce::MouseEvent &) override;
};
class PrismSource : public juce::TextButton
{
    int source;

  public:
    PrismSource(int s) : juce::TextButton(prism::sourceNames[size_t(s)]), source(s) {}
    void mouseDrag(const juce::MouseEvent &e) override
    {
        if (e.getDistanceFromDragStart() > 4)
            if (auto *container = juce::DragAndDropContainer::findParentDragContainerFor(this))
                container->startDragging(juce::String("prism-source:") + juce::String(source),
                                         this);
    }
};
class PrismDetails : public juce::Component
{
  public:
    struct Group
    {
        juce::String title;
        std::vector<int> ids;
    };

  private:
    PrismProcessor &processor;
    std::vector<Group> groups;
    std::vector<std::unique_ptr<PrismControl>> controls;
    std::vector<juce::Rectangle<int>> panels;
    std::vector<std::unique_ptr<PrismSource>> sources;

  public:
    explicit PrismDetails(PrismProcessor &p) : processor(p) {}
    int groupY(const juce::String &prefix) const
    {
        for (size_t i = 0; i < groups.size(); ++i)
            if (groups[i].title.startsWith(prefix))
                return panels[i].getY();
        return 0;
    }
    void setGroups(std::vector<Group>);
    void resized() override;
    void paint(juce::Graphics &) override;
    void refreshEnabled()
    {
        for (auto &c : controls)
            c->refreshEnabled();
    }
};
class PrismEditor : public juce::AudioProcessorEditor,
                    public juce::DragAndDropContainer,
                    private juce::Timer
{
    PrismProcessor &processor;
    PrismLookAndFeel look;
    std::array<std::unique_ptr<PrismControl>, prism::paramCount> controls;
    std::array<std::unique_ptr<PrismSource>, 14> quickSources;
    std::array<juce::TextButton, 10> tabs;
    std::array<juce::TextButton, 3> loadButtons;
    std::unique_ptr<juce::FileChooser> assetChooser;
    juce::Label assetMessage;
    unsigned lastAssetGeneration = 0;
    int waveformDrag = -1;
    float dragWaveX = 0, dragWaveWidth = 1;
    int page = 0, envelopeIndex = 0;
    juce::TextButton programs;
    juce::ComboBox envelopes;
    juce::Label status;
    juce::TextButton legal{"0.3.1 / LICENSE"}, panic{"PANIC"}, previous{"<"}, next{">"};
    juce::MidiKeyboardComponent keys;
    juce::TooltipWindow tooltip{this, 700};
    juce::Viewport viewport;
    PrismDetails details;
    std::unique_ptr<juce::TextEditor> legalText;
    std::unique_ptr<PrismBrowser> browser;
    std::unique_ptr<PrismTableEditor> tableEditor;
    std::unique_ptr<PrismScope> scope;
    std::array<float, prism::paramCount> lastValues{};
    float lastPeak = 0;
    bool visualInitialised = false;
    void timerCallback() override;
    void selectPage(int);
    void place(juce::Component &, int, int, int, int);
    void control(int, int, int, int, int);
    void drawWave(juce::Graphics &, int);
    void drawEnvelope(juce::Graphics &);
    void drawFilter(juce::Graphics &);

  public:
    explicit PrismEditor(PrismProcessor &);
    ~PrismEditor() override;
    void paint(juce::Graphics &) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent &) override;
    void mouseDrag(const juce::MouseEvent &) override;
    void mouseUp(const juce::MouseEvent &) override;
};
