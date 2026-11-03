#include "PresetBrowser.h"
juce::File PrismBrowser::userDirectory() const
{
    return juce::File::getSpecialLocation(juce::File::userMusicDirectory)
        .getChildFile("Prism Forge/Presets");
}
PrismBrowser::PrismBrowser(PrismProcessor &p) : processor(p)
{
    for (auto *c : std::array<juce::Component *, 20>{
             &list,        &search,        &name,           &category,
             &tags,        &author,        &description,    &categoryViewport,
             &resultCount, &detailsToggle, &favouritesOnly, &save,
             &rename,      &duplicate,     &import,         &exportButton,
             &favourite,   &previous,      &next,           &message})
        addAndMakeVisible(c);
    categoryViewport.setViewedComponent(&categoryPanel, false);
    categoryViewport.setScrollBarsShown(true, false);
    categoryViewport.setName("Preset categories");
    search.setName("Search presets");
    list.setName("Categorized presets");
    detailsToggle.onClick = [this]
    {
        showDetails = !showDetails;
        detailsToggle.setButtonText(showDetails ? "Hide details" : "Edit details");
        resized();
        repaint();
    };
    for (auto &field : macroNames)
    {
        addAndMakeVisible(field);
        field.setName("Assigned macro name");
    }
    search.setTextToShowWhenEmpty("Search names, categories, tags and descriptions",
                                  juce::Colours::grey);
    search.onTextChange = [this] { filter(); };
    categoryFilter.onChange = [this] { filter(); };
    favouritesOnly.onClick = [this] { filter(); };
    list.setRowHeight(47);
    auto setup = [](juce::TextEditor &field, const char *label)
    {
        field.setName(label);
        field.setTextToShowWhenEmpty(label, juce::Colours::grey);
    };
    setup(name, "Preset name");
    setup(category, "Category");
    setup(tags, "Tags");
    setup(author, "Author");
    setup(description, "Description / suggested playing range");
    description.setMultiLine(true);
    save.onClick = [this] { choose(true); };
    exportButton.onClick = [this] { choose(true); };
    import.onClick = [this] { choose(false); };
    previous.onClick = [this]
    {
        int r = list.getSelectedRow();
        if (!filtered.empty())
            list.selectRow(r < 0 ? int(filtered.size()) - 1
                                 : (r + int(filtered.size()) - 1) % int(filtered.size()));
    };
    next.onClick = [this]
    {
        if (!filtered.empty())
            list.selectRow((list.getSelectedRow() + 1) % int(filtered.size()));
    };
    favourite.onClick = [this]
    {
        int row = list.getSelectedRow();
        if (row < 0 || row >= int(filtered.size()))
            return;
        auto &e = entries[filtered[size_t(row)]];
        auto key = e.factory >= 0 ? juce::String("factory:") + juce::String(e.factory)
                                  : e.file.getFullPathName();
        if (favourites.contains(key))
            favourites.removeString(key);
        else
            favourites.add(key);
        juce::Array<juce::var> a;
        for (auto s : favourites)
            a.add(s);
        userDirectory().createDirectory();
        userDirectory()
            .getChildFile("favourites.json")
            .replaceWithText(juce::JSON::toString(juce::var(a)));
        filter();
    };
    duplicate.onClick = [this]
    {
        userDirectory().createDirectory();
        name.setText(name.getText() + " Copy", false);
        write(userDirectory().getNonexistentChildFile(
            juce::File::createLegalFileName(name.getText()), ".prism"));
        scan();
    };
    rename.onClick = [this]
    {
        int row = list.getSelectedRow();
        if (row < 0 || row >= int(filtered.size()))
            return;
        auto e = entries[filtered[size_t(row)]];
        if (e.factory >= 0)
        {
            message.setText("Factory files are protected. Use Duplicate or Save As.",
                            juce::dontSendNotification);
            return;
        }
        auto destination =
            e.file.getSiblingFile(juce::File::createLegalFileName(name.getText()) + ".prism");
        if (destination != e.file && destination.exists())
        {
            message.setText("That name already exists.", juce::dontSendNotification);
            return;
        }
        if (e.file == destination || e.file.moveFileTo(destination))
        {
            write(destination);
            scan();
        }
    };
    auto fav = juce::JSON::parse(userDirectory().getChildFile("favourites.json"));
    if (auto *array = fav.getArray())
        for (auto v : *array)
            favourites.add(v.toString());
    scan();
    updateFields();
}
void PrismBrowser::scan()
{
    entries.clear();
    for (size_t i = 0; i < prism::presets.size(); ++i)
    {
        auto &p = prism::presets[i];
        entries.push_back({p.name, p.category, p.tags, p.description, int(i), {}});
    }
    for (auto file : userDirectory().findChildFiles(juce::File::findFiles, false, "*.prism"))
    {
        if (file.getSize() > 64 * 1024 * 1024)
            continue;
        juce::MemoryBlock bytes;
        if (!file.loadFileAsData(bytes))
            continue;
        auto xml = juce::AudioProcessor::getXmlFromBinary(bytes.getData(), int(bytes.getSize()));
        if (xml && xml->hasTagName("PrismForge"))
            entries.push_back(
                {xml->getStringAttribute("presetName", file.getFileNameWithoutExtension()),
                 xml->getStringAttribute("category", "User"), xml->getStringAttribute("tags"),
                 xml->getStringAttribute("description"), -1, file});
    }
    std::stable_sort(entries.begin(), entries.end(),
                     [](const auto &a, const auto &b)
                     {
                         const int categoryOrder = a.category.compareNatural(b.category);
                         return categoryOrder ? categoryOrder < 0
                                              : a.name.compareNatural(b.name) < 0;
                     });
    auto selected = categoryFilter.getText();
    juce::StringArray categories;
    for (auto &e : entries)
        categories.addIfNotAlreadyThere(e.category);
    categories.sort(true);
    categoryFilter.clear(juce::dontSendNotification);
    categoryFilter.addItem("All categories", 1);
    int id = 2;
    for (auto c : categories)
        categoryFilter.addItem(c, id++);
    categoryFilter.setSelectedId(1, juce::dontSendNotification);
    if (selected.isNotEmpty())
        categoryFilter.setText(selected, juce::dontSendNotification);
    categoryButtons.clear();
    for (int item = 0; item < categoryFilter.getNumItems(); ++item)
    {
        const auto label = categoryFilter.getItemText(item);
        const int id = categoryFilter.getItemId(item);
        int count = 0;
        for (const auto &entry : entries)
            if (id == 1 || entry.category == label)
                ++count;
        auto button = std::make_unique<juce::TextButton>((id == 1 ? "All sounds" : label) + "  " +
                                                         juce::String(count));
        button->setName((id == 1 ? "All sounds" : label) + " presets");
        button->onClick = [this, id]
        { categoryFilter.setSelectedId(id, juce::sendNotificationSync); };
        categoryPanel.addAndMakeVisible(*button);
        categoryButtons.push_back(std::move(button));
    }
    resized();
    filter();
}
void PrismBrowser::filter()
{
    filtered.clear();
    auto term = search.getText().trim();
    for (size_t i = 0; i < entries.size(); ++i)
    {
        auto &e = entries[i];
        auto key = e.factory >= 0 ? juce::String("factory:") + juce::String(e.factory)
                                  : e.file.getFullPathName();
        if (favouritesOnly.getToggleState() && !favourites.contains(key))
            continue;
        if (categoryFilter.getSelectedId() > 1 && categoryFilter.getText() != e.category)
            continue;
        if (term.isNotEmpty() && !(e.name + " " + e.category + " " + e.tags + " " + e.description)
                                      .containsIgnoreCase(term))
            continue;
        filtered.push_back(i);
    }
    for (size_t i = 0; i < categoryButtons.size(); ++i)
        categoryButtons[i]->setToggleState(categoryFilter.getSelectedId() ==
                                               categoryFilter.getItemId(int(i)),
                                           juce::dontSendNotification);
    resultCount.setText(juce::String(filtered.size()) +
                            (filtered.size() == 1 ? " sound" : " sounds") +
                            (filtered.empty() ? " — clear search or choose another category"
                                              : " — click a sound to load"),
                        juce::dontSendNotification);
    list.deselectAllRows();
    list.updateContent();
    list.repaint();
}
void PrismBrowser::paintListBoxItem(int row, juce::Graphics &g, int w, int h, bool selected)
{
    if (row < 0 || row >= int(filtered.size()))
        return;
    auto &e = entries[filtered[size_t(row)]];
    g.fillAll(juce::Colour(selected ? 0xff45395c : 0xff252033));
    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(14));
    g.drawText(e.name, 10, 2, w - 20, 23, juce::Justification::centredLeft);
    g.setColour(juce::Colour(0xffa4b2be));
    g.setFont(juce::FontOptions(10));
    g.drawText(e.category + " / " + e.description, 10, 25, w - 20, h - 26,
               juce::Justification::centredLeft);
}
juce::String PrismBrowser::getNameForRow(int row)
{
    if (row < 0 || row >= int(filtered.size()))
        return {};
    const auto &entry = entries[filtered[size_t(row)]];
    return entry.name + " / " + entry.category;
}
void PrismBrowser::selectedRowsChanged(int row)
{
    load(row);
}
void PrismBrowser::load(int row)
{
    if (row < 0 || row >= int(filtered.size()))
        return;
    auto &e = entries[filtered[size_t(row)]];
    if (e.factory >= 0)
        processor.setCurrentProgram(e.factory);
    else
    {
        juce::MemoryBlock bytes;
        if (!e.file.loadFileAsData(bytes) ||
            !processor.restoreState(bytes.getData(), int(bytes.getSize())))
        {
            message.setText("Preset is invalid or uses an unsupported version.",
                            juce::dontSendNotification);
            return;
        }
    }
    updateFields();
    message.setText(e.description, juce::dontSendNotification);
}
void PrismBrowser::updateFields()
{
    name.setText(processor.presetText("presetName"), false);
    category.setText(processor.presetText("category"), false);
    tags.setText(processor.presetText("tags"), false);
    author.setText(processor.presetText("author"), false);
    description.setText(processor.presetText("description"), false);
    for (int i = 0; i < 4; ++i)
        macroNames[size_t(i)].setText(processor.presetText("macroName" + juce::String(i + 1)),
                                      false);
}
void PrismBrowser::editMetadata()
{
    processor.setPresetMetadata(name.getText().trim(), category.getText().trim(), tags.getText(),
                                author.getText(), description.getText());
    std::array<juce::String, 4> names;
    for (int i = 0; i < 4; ++i)
        names[size_t(i)] = macroNames[size_t(i)].getText();
    processor.setMacroNames(names);
}
bool PrismBrowser::write(const juce::File &file)
{
    if (name.getText().trim().isEmpty())
    {
        message.setText("Enter a preset name first.", juce::dontSendNotification);
        return false;
    }
    editMetadata();
    juce::MemoryBlock data;
    processor.getStateInformation(data);
    bool ok = file.replaceWithData(data.getData(), data.getSize());
    message.setText(ok ? "Saved: " + file.getFullPathName() : "Could not write the preset file.",
                    juce::dontSendNotification);
    return ok;
}
void PrismBrowser::choose(bool saving)
{
    userDirectory().createDirectory();
    chooser = std::make_unique<juce::FileChooser>(
        saving ? "Save Prism Forge preset" : "Import Prism Forge preset",
        saving ? userDirectory().getChildFile(juce::File::createLegalFileName(name.getText()) +
                                              ".prism")
               : userDirectory(),
        "*.prism");
    juce::Component::SafePointer<PrismBrowser> safe(this);
    int flags = juce::FileBrowserComponent::canSelectFiles |
                (saving ? juce::FileBrowserComponent::saveMode |
                              juce::FileBrowserComponent::warnAboutOverwriting
                        : juce::FileBrowserComponent::openMode);
    chooser->launchAsync(
        flags,
        [safe, saving](const juce::FileChooser &dialog)
        {
            if (!safe)
                return;
            auto f = dialog.getResult();
            if (f == juce::File{})
                return;
            if (saving)
                safe->write(f.withFileExtension("prism"));
            else
            {
                juce::MemoryBlock bytes;
                if (f.getSize() > 64 * 1024 * 1024 || !f.loadFileAsData(bytes) ||
                    !safe->processor.restoreState(bytes.getData(), int(bytes.getSize())))
                {
                    safe->message.setText("Invalid preset; the sound was not replaced.",
                                          juce::dontSendNotification);
                    return;
                }
                safe->updateFields();
                auto destination = safe->userDirectory().getNonexistentChildFile(
                    f.getFileNameWithoutExtension(), ".prism");
                safe->write(destination);
            }
            safe->scan();
        });
}
void PrismBrowser::paint(juce::Graphics &g)
{
    g.fillAll(juce::Colour(0xff181522));
    g.setColour(juce::Colour(0xffb0a3bf));
    g.setFont(juce::FontOptions(11));
    g.drawText("CATEGORIES", 12, 52, 156, 22, juce::Justification::centredLeft);
    if (!showDetails)
        return;
    int x = getWidth() * 3 / 5 + 18;
    g.drawText("PRESET DETAILS", x, 52, getWidth() - x - 12, 22, juce::Justification::centredLeft);
    const std::array<const char *, 5> labels{"Name", "Category", "Tags", "Author", "Description"};
    const std::array<int, 5> ys{84, 121, 156, 191, 226};
    for (int i = 0; i < 5; ++i)
        g.drawText(labels[size_t(i)], x, ys[size_t(i)], 78, 27, juce::Justification::centredLeft);
}
void PrismBrowser::resized()
{
    int listRight = showDetails ? getWidth() * 3 / 5 : getWidth();
    search.setBounds(180, 12, listRight - 348, 28);
    favouritesOnly.setBounds(listRight - 160, 12, 148, 28);
    detailsToggle.setBounds(12, 12, 156, 28);
    categoryViewport.setBounds(12, 80, 156, getHeight() - 92);
    categoryPanel.setSize(142,
                          std::max(categoryViewport.getHeight(), int(categoryButtons.size()) * 34));
    for (size_t i = 0; i < categoryButtons.size(); ++i)
        categoryButtons[i]->setBounds(0, int(i) * 34, 142, 30);
    resultCount.setBounds(180, 48, listRight - 192, 28);
    list.setBounds(180, 80, listRight - 192, getHeight() - 134);
    previous.setBounds(180, getHeight() - 44, 66, 28);
    next.setBounds(252, getHeight() - 44, 66, 28);
    favourite.setBounds(listRight - 94, getHeight() - 44, 82, 28);
    int x = getWidth() * 3 / 5 + 18, w = getWidth() - x - 12;
    std::array<juce::Component *, 16> detailControls{
        &name,          &category,      &tags,          &author,
        &description,   &macroNames[0], &macroNames[1], &macroNames[2],
        &macroNames[3], &save,          &rename,        &duplicate,
        &import,        &exportButton,  &message,       &categoryFilter};
    for (auto *component : detailControls)
        component->setVisible(showDetails);
    categoryFilter.setVisible(false);
    name.setBounds(x + 82, 84, w - 82, 29);
    category.setBounds(x + 82, 121, w - 82, 27);
    tags.setBounds(x + 82, 156, w - 82, 27);
    author.setBounds(x + 82, 191, w - 82, 27);
    description.setBounds(x + 82, 226, w - 82, 56);
    for (int i = 0; i < 4; ++i)
        macroNames[size_t(i)].setBounds(x + (i % 2) * (w / 2), 292 + (i / 2) * 31, w / 2 - 5, 27);
    std::array<juce::TextButton *, 3> buttons{&rename, &duplicate, &exportButton};
    for (int i = 0; i < 3; ++i)
        buttons[size_t(i)]->setBounds(x + (i % 3) * (w / 3), 366 + (i / 3) * 38, w / 3 - 5, 30);
    message.setBounds(x, 410, w, juce::jlimit(0, 70, getHeight() - 422));
    import.setVisible(true);
    save.setVisible(true);
    import.setBounds(324, getHeight() - 44, 70, 28);
    save.setBounds(400, getHeight() - 44, 76, 28);
}
