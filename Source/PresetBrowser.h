#pragma once
#include "Plugin.h"
class PrismBrowser : public juce::Component, private juce::ListBoxModel
{
    PrismProcessor &processor;
    struct Entry
    {
        juce::String name, category, tags, description;
        int factory = -1;
        juce::File file;
    };
    std::vector<Entry> entries;
    std::vector<size_t> filtered;
    juce::StringArray favourites;
    juce::ListBox list{"Presets", this};
    juce::TextEditor search, name, category, tags, author, description;
    std::array<juce::TextEditor, 4> macroNames;
    juce::ComboBox categoryFilter;
    juce::Component categoryPanel;
    juce::Viewport categoryViewport;
    std::vector<std::unique_ptr<juce::TextButton>> categoryButtons;
    juce::Label resultCount;
    juce::TextButton detailsToggle{"Edit details"};
    bool showDetails = false;
    juce::ToggleButton favouritesOnly{"Favourites only"};
    juce::TextButton save{"Save As"}, rename{"Rename"}, duplicate{"Duplicate"}, import{"Import"},
        exportButton{"Export"}, favourite{"Favourite"}, previous{"Previous"}, next{"Next"};
    juce::Label message;
    std::unique_ptr<juce::FileChooser> chooser;
    juce::File userDirectory() const;
    void scan();
    void filter();
    void load(int);
    void editMetadata();
    bool write(const juce::File &);
    void choose(bool);
    void updateFields();
    int getNumRows() override
    {
        return int(filtered.size());
    }
    void paintListBoxItem(int, juce::Graphics &, int, int, bool) override;
    void selectedRowsChanged(int) override;
    juce::String getNameForRow(int) override;

  public:
    explicit PrismBrowser(PrismProcessor &);
    void resized() override;
    void paint(juce::Graphics &) override;
};
