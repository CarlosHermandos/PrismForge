#include "Editor.h"
#include "BinaryData.h"
#include <complex>
namespace
{
const juce::Colour background(0xff181522), panel(0xff252033), border(0xff100e18),
    header(0xff45395c), text(0xffeee7f5), muted(0xffb0a3bf), green(0xffffb66b), blue(0xffb69aff);
void word(juce::Graphics &g, juce::String s, float x, float y, float w, float h, float size,
          juce::Colour c, juce::Justification align = juce::Justification::centredLeft)
{
    g.setColour(c);
    g.setFont(juce::FontOptions(size));
    g.drawText(s, juce::Rectangle<float>(x, y, w, h), align);
}
void box(juce::Graphics &g, float x, float y, float w, float h, juce::String title,
         juce::Colour accent, bool enabled = true)
{
    g.setColour(enabled ? panel : juce::Colour(0xff1c2025));
    g.fillRoundedRectangle(x, y, w, h, 8.f);
    g.setColour(border);
    g.drawRoundedRectangle(x, y, w, h, 8.f, 1.f);
    g.setColour(enabled ? header : juce::Colour(0xff394149));
    g.fillRoundedRectangle(x + 1, y + 1, w - 2, 25.f, 7.f);
    g.setColour(accent.withAlpha(enabled ? 1.f : .25f));
    g.fillRoundedRectangle(x + 9, y + 9, 7, 9, 2);
    word(g, title, x + 23, y + 1, w - 30, 25, 11, enabled ? text : muted);
}
void grid(juce::Graphics &g, juce::Rectangle<float> r)
{
    g.setColour(juce::Colour(0xff303a42));
    for (int i = 1; i < 12; ++i)
        g.drawVerticalLine(int(r.getX() + r.getWidth() * i / 12), r.getY(), r.getBottom());
    for (int i = 1; i < 5; ++i)
        g.drawHorizontalLine(int(r.getY() + r.getHeight() * i / 5), r.getX(), r.getRight());
}
} // namespace
PrismLookAndFeel::PrismLookAndFeel()
{
    setColour(juce::Slider::textBoxTextColourId, text);
    setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xff181e23));
    setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    setColour(juce::Slider::rotarySliderFillColourId, blue);
    setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff191f25));
    setColour(juce::ComboBox::outlineColourId, border);
    setColour(juce::ComboBox::textColourId, text);
    setColour(juce::PopupMenu::backgroundColourId, panel);
    setColour(juce::TextButton::textColourOffId, text);
}
void PrismLookAndFeel::drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height,
                                        float position, float start, float end,
                                        juce::Slider &slider)
{
    float diameter = juce::jmin(float(width) - 10, float(height) - 7, 44.f), r = diameter * .5f,
          cx = x + width * .5f, cy = y + height * .5f;
    juce::Path arc;
    arc.addCentredArc(cx, cy, r + 3, r + 3, 0, start, end, true);
    g.setColour(juce::Colour(0xff11171c));
    g.strokePath(arc, juce::PathStrokeType(3));
    juce::Path amount;
    amount.addCentredArc(cx, cy, r + 3, r + 3, 0, start, start + position * (end - start), true);
    g.setColour(slider.findColour(juce::Slider::rotarySliderFillColourId));
    g.strokePath(amount, juce::PathStrokeType(2));
    g.setColour(panel.brighter(.1f));
    g.fillEllipse(cx - r, cy - r, diameter, diameter);
    juce::Path facet;
    for (int corner = 0; corner < 6; ++corner)
    {
        float a = juce::MathConstants<float>::twoPi * corner / 6.f;
        auto point = juce::Point<float>(cx + std::sin(a) * r * .83f, cy - std::cos(a) * r * .83f);
        if (corner == 0)
            facet.startNewSubPath(point);
        else
            facet.lineTo(point);
    }
    facet.closeSubPath();
    g.setColour(blue.withAlpha(.35f));
    g.strokePath(facet, juce::PathStrokeType(1.f));
    float angle = start + position * (end - start);
    g.setColour(text);
    g.drawLine(cx + std::sin(angle) * r * .48f, cy - std::cos(angle) * r * .48f,
               cx + std::sin(angle) * r * .85f, cy - std::cos(angle) * r * .85f, 2);
}
void PrismLookAndFeel::drawButtonBackground(juce::Graphics &g, juce::Button &b,
                                            const juce::Colour &, bool over, bool down)
{
    g.setColour(b.getToggleState() ? juce::Colour(0xff394c5c)
                                   : juce::Colour(over ? 0xff34414b : 0xff252d35));
    if (down)
        g.setColour(juce::Colour(0xff40505d));
    g.fillRect(b.getLocalBounds());
    g.setColour(border);
    g.drawRect(b.getLocalBounds());
    if (b.getToggleState())
    {
        g.setColour(blue);
        g.fillRect(0, b.getHeight() - 2, b.getWidth(), 2);
    }
}
PrismControl::PrismControl(PrismProcessor &p, size_t i) : processor(p), parameter(i)
{
    const auto &d = prism::parameters[i];
    auto shortName = juce::String(d.name);
    if (juce::String(d.id).startsWith("osc") && shortName.startsWith("Osc "))
        shortName = shortName.substring(6);
    label.setText(shortName, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.setFont(juce::FontOptions(10));
    label.setColour(juce::Label::textColourId, muted);
    addAndMakeVisible(label);
    label.addMouseListener(this, false);
    setName(d.name);
    if (d.step > 0 && prism::choiceName(i, int(d.min)))
    {
        for (int v = int(d.min); v <= int(d.max); ++v)
            choice.addItem(prism::choiceName(i, v), v - int(d.min) + 1);
        choice.setName(d.name);
        addAndMakeVisible(choice);
        choiceAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
            p.state, d.id, choice);
    }
    else
    {
        slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setVelocityModeParameters(0.5, 1, 0.0, true, juce::ModifierKeys::shiftModifier);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 18);
        slider.setTextValueSuffix(juce::String(" ") + d.unit);
        slider.setDoubleClickReturnValue(true, d.def);
        slider.setName(d.name);
        slider.setTooltip(juce::String(d.name) +
                          " / Shift-drag: fine / Double-click: reset / Click value: type");
        slider.setWantsKeyboardFocus(true);
        addAndMakeVisible(slider);
        attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            p.state, d.id, slider);
    }
    if (i == prism::osc1_interaction || i == prism::osc1_interactionAmount)
    {
        setEnabled(false);
        slider.setTooltip("Oscillator A has no preceding modulation oscillator");
    }
}
void PrismControl::refreshEnabled()
{
    bool enabled = true;
    for (int o = 0; o < 3; ++o)
    {
        auto indices = prism::oscillatorIndices[size_t(o)];
        int mode = int(processor.values[size_t(prism::hybrid1_mode + o * 18)]->load());
        for (int id : {indices.table, indices.unison, indices.detune, indices.spread, indices.phase,
                       indices.random, indices.warp, indices.warpAmount, indices.interaction,
                       indices.interactionAmount})
            if (parameter == size_t(id) && mode != 0)
                enabled = false;
        if ((parameter == size_t(prism::osc1_warp2 + o * 2) ||
             parameter == size_t(prism::osc1_warp2Amount + o * 2)) &&
            mode != 0)
            enabled = false;
        if (parameter == size_t(prism::hybrid1_oneShot + o) && (mode != 1 && mode != 2))
            enabled = false;
        int base = prism::hybrid1_mode + o * 18;
        if (parameter > size_t(base) && parameter < size_t(base + 18))
        {
            int offset = int(parameter) - base;
            if (mode == 0)
                enabled = false;
            if (offset >= 8 && offset <= 14 && mode != 3)
                enabled = false;
            if (offset >= 15 && mode != 4)
                enabled = false;
            if (offset >= 4 && offset <= 6 && mode != 1 && mode != 2)
                enabled = false;
            if (offset == 1 && mode == 2)
                enabled = false;
        }
    }
    if (parameter >= prism::lfo1_point1 && parameter <= prism::lfo10_point8)
    {
        int l = int(parameter - prism::lfo1_point1) / 8;
        if (processor.values[size_t(prism::lfo1_shape + l * 8)]->load() != 5)
            enabled = false;
    }
    if (parameter == prism::osc1_interaction || parameter == prism::osc1_interactionAmount)
        enabled = false;
    if (parameter >= prism::macro1 && parameter < prism::macro1 + 4)
    {
        auto caption =
            processor.presetText("macroName" + juce::String(int(parameter - prism::macro1) + 1));
        if (caption.isNotEmpty())
            label.setText(caption, juce::dontSendNotification);
    }
    setEnabled(enabled);
}
void PrismControl::paintOverChildren(juce::Graphics &g)
{
    int dest = prism::modulationTarget(parameter);
    if (!dest)
        return;
    float depth = 0;
    int count = 0;
    for (int route = 0; route < 16; ++route)
    {
        int base = prism::mod1_source + route * 6;
        if (int(processor.values[size_t(base + 1)]->load()) == dest &&
            processor.values[size_t(base)]->load() > 0 &&
            processor.values[size_t(base + 4)]->load() < .5f)
        {
            depth += processor.values[size_t(base + 2)]->load();
            ++count;
        }
    }
    if (count)
    {
        g.setColour(juce::Colour(0xffb69aff));
        g.fillEllipse(float(getWidth() - 7), 3, 4, 4);
        g.drawLine(float(getWidth() - 24), 11,
                   float(getWidth() - 24) + juce::jlimit(-1.f, 1.f, depth) * 18, 11, 2);
    }
}
void PrismControl::mouseDrag(const juce::MouseEvent &event)
{
    if (parameter >= prism::macro1 && parameter <= prism::macro8 &&
        event.getDistanceFromDragStart() > 4)
        if (auto *container = juce::DragAndDropContainer::findParentDragContainerFor(this))
            container->startDragging(juce::String("prism-source:") +
                                         juce::String(22 + int(parameter - prism::macro1)),
                                     this);
}
bool PrismControl::isInterestedInDragSource(const SourceDetails &source)
{
    if (!source.description.toString().startsWith("prism-source:"))
        return false;
    for (auto o : prism::oscillatorIndices)
        if (parameter == size_t(o.table) || parameter == size_t(o.pitch) ||
            parameter == size_t(o.level) || parameter == size_t(o.pan) ||
            parameter == size_t(o.warpAmount))
            return true;
    return parameter == prism::cutoff || parameter == prism::resonance ||
           parameter == prism::filter2_cutoff || parameter == prism::filter2_res ||
           parameter == prism::sub_level || parameter == prism::noise_level;
}
void PrismControl::itemDropped(const SourceDetails &source)
{
    int dest = 0;
    for (int o = 0; o < 3; ++o)
    {
        auto indices = prism::oscillatorIndices[size_t(o)];
        std::array<int, 5> ids{indices.table, indices.pitch, indices.level, indices.pan,
                               indices.warpAmount};
        for (int i = 0; i < 5; ++i)
            if (parameter == size_t(ids[size_t(i)]))
                dest = 1 + o * 5 + i;
    }
    if (parameter == prism::cutoff)
        dest = 16;
    if (parameter == prism::resonance)
        dest = 17;
    if (parameter == prism::filter2_cutoff)
        dest = 18;
    if (parameter == prism::filter2_res)
        dest = 19;
    if (parameter == prism::sub_level)
        dest = 20;
    if (parameter == prism::noise_level)
        dest = 21;
    int src = source.description.toString().fromFirstOccurrenceOf(":", false, false).getIntValue();
    if (!dest || src < 1 || src > 29)
        return;
    for (int r = 0; r < 16; ++r)
    {
        int base = prism::mod1_source + r * 6;
        if (processor.values[size_t(base)]->load() == 0 ||
            processor.values[size_t(base + 1)]->load() == 0)
        {
            auto set = [&](int id, float value)
            {
                auto *p = processor.state.getParameter(prism::parameters[size_t(id)].id);
                p->beginChangeGesture();
                p->setValueNotifyingHost(p->convertTo0to1(value));
                p->endChangeGesture();
            };
            set(base, float(src));
            set(base + 1, float(dest));
            set(base + 2, .25f);
            return;
        }
    }
}
void PrismControl::resized()
{
    label.setBounds(0, 0, getWidth(), 15);
    slider.setBounds(3, 15, getWidth() - 6, getHeight() - 15);
    choice.setBounds(5, 29, getWidth() - 10, 24);
}
void PrismDetails::setGroups(std::vector<Group> g)
{
    controls.clear();
    sources.clear();
    groups = std::move(g);
    if (!groups.empty() && groups[0].title.startsWith("MODULATION"))
        for (int source = 1; source <= 29; ++source)
        {
            auto tile = std::make_unique<PrismSource>(source);
            addAndMakeVisible(*tile);
            sources.push_back(std::move(tile));
        }
    for (auto &group : groups)
        for (int id : group.ids)
        {
            auto c = std::make_unique<PrismControl>(processor, size_t(id));
            addAndMakeVisible(*c);
            controls.push_back(std::move(c));
        }
    resized();
    repaint();
}
void PrismDetails::resized()
{
    int columns = std::max(3, getWidth() / 155), y = sources.empty() ? 8 : 146, index = 0;
    panels.clear();
    for (size_t i = 0; i < sources.size(); ++i)
    {
        int w = (getWidth() - 24) / 8;
        sources[i]->setBounds(12 + int(i % 8) * w, 26 + int(i / 8) * 27, w - 4, 24);
    }
    for (auto &group : groups)
    {
        int rows = (int(group.ids.size()) + columns - 1) / columns, h = 32 + rows * 94;
        panels.emplace_back(6, y, getWidth() - 12, h);
        for (size_t i = 0; i < group.ids.size(); ++i)
        {
            int w = (getWidth() - 32) / columns;
            controls[size_t(index++)]->setBounds(16 + int(i % size_t(columns)) * w,
                                                 y + 30 + int(i / size_t(columns)) * 94, w - 5, 88);
        }
        y += h + 10;
    }
    if (getHeight() != y)
        setSize(getWidth(), y);
}
void PrismDetails::paint(juce::Graphics &g)
{
    g.fillAll(background);
    if (!sources.empty())
        word(g, "DRAG A SOURCE ONTO A DESTINATION CONTROL / FIRST EMPTY ROUTE, DEPTH +0.25", 14, 3,
             float(getWidth() - 28), 20, 10, blue);
    for (size_t i = 0; i < panels.size(); ++i)
    {
        auto r = panels[i];
        box(g, float(r.getX()), float(r.getY()), float(r.getWidth()), float(r.getHeight()),
            groups[i].title, blue);
    }
}
PrismEditor::PrismEditor(PrismProcessor &p)
    : AudioProcessorEditor(p), processor(p),
      keys(p.keyboard, juce::MidiKeyboardComponent::horizontalKeyboard), details(p)
{
    setLookAndFeel(&look);
    for (int i = 0; i < 14; ++i)
    {
        quickSources[size_t(i)] = std::make_unique<PrismSource>(i + 1);
        quickSources[size_t(i)]->onClick = [this, i]
        {
            if (i < 4)
            {
                envelopes.setSelectedId(i + 1, juce::dontSendNotification);
                envelopeIndex = i;
                selectPage(0);
            }
            else
            {
                selectPage(3);
                viewport.setViewPosition(0, details.groupY("LFO " + juce::String(i - 3) + " /"));
            }
        };
        addAndMakeVisible(*quickSources[size_t(i)]);
    }
    const std::array<const char *, 10> names{"OSC",   "MIX",   "FX",    "MOD", "SETUP",
                                             "PATCH", "TABLE", "ASSET", "ARP", "SCOPE"};
    for (size_t i = 0; i < tabs.size(); ++i)
    {
        tabs[i].setButtonText(names[i]);
        tabs[i].onClick = [this, i] { selectPage(int(i)); };
        addAndMakeVisible(tabs[i]);
    }
    for (int o = 0; o < 3; ++o)
    {
        loadButtons[size_t(o)].setButtonText("Load OSC " +
                                             juce::String::charToString(juce::juce_wchar('A' + o)));
        loadButtons[size_t(o)].onClick = [this, o]
        {
            int mode = int(processor.values[size_t(prism::hybrid1_mode + o * 18)]->load());
            assetChooser = std::make_unique<juce::FileChooser>(
                "Load oscillator asset", juce::File{},
                mode == 2 ? "*.sfz" : "*.wav;*.aif;*.aiff;*.flac");
            juce::Component::SafePointer<PrismEditor> safe(this);
            assetChooser->launchAsync(
                juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
                [safe, o, mode](const juce::FileChooser &chooser)
                {
                    if (!safe || !chooser.getResult().existsAsFile())
                        return;
                    if (mode == 0)
                        safe->processor.loadWavetable(o, chooser.getResult(), 2048, true);
                    else
                        safe->processor.loadHybrid(o, chooser.getResult(), mode);
                });
        };
        addChildComponent(loadButtons[size_t(o)]);
    }
    addChildComponent(assetMessage);
    assetMessage.setColour(juce::Label::textColourId, blue);
    programs.setName("Browse presets");
    programs.setTooltip(
        "Open the preset browser. Choose a category, then click a sound to load it.");
    programs.onClick = [this] { selectPage(5); };
    addAndMakeVisible(programs);
    previous.onClick = [this]
    {
        processor.setCurrentProgram(
            (processor.getCurrentProgram() + processor.getNumPrograms() - 1) %
            processor.getNumPrograms());
        timerCallback();
    };
    next.onClick = [this]
    {
        processor.setCurrentProgram((processor.getCurrentProgram() + 1) %
                                    processor.getNumPrograms());
        timerCallback();
    };
    addAndMakeVisible(previous);
    addAndMakeVisible(next);
    for (int e = 1; e <= 4; ++e)
        envelopes.addItem("Envelope " + juce::String(e), e);
    envelopes.setSelectedId(1);
    envelopes.onChange = [this]
    {
        envelopeIndex = envelopes.getSelectedId() - 1;
        resized();
        repaint();
    };
    addAndMakeVisible(envelopes);
    panic.onClick = [this]
    {
        for (int c = 1; c <= 16; ++c)
            processor.enqueue(0xb0 + c - 1, 120, 0);
    };
    addAndMakeVisible(panic);
    legal.setName("About Prism Forge and AGPLv3 license");
    legal.onClick = [this]
    {
        if (legalText)
        {
            legalText.reset();
            legal.setButtonText("0.3.1 / LICENSE");
            return;
        }
        legalText = std::make_unique<juce::TextEditor>();
        legalText->setName("Prism Forge license and notices");
        legalText->setMultiLine(true);
        legalText->setReadOnly(true);
        legalText->setScrollbarsShown(true);
        legalText->setText(
            juce::String::fromUTF8(PrismLegalData::NOTICE_md, PrismLegalData::NOTICE_mdSize) +
            "\n\n" +
            juce::String::fromUTF8(PrismLegalData::LICENSE_md, PrismLegalData::LICENSE_mdSize));
        legalText->setCaretPosition(0);
        addAndMakeVisible(*legalText);
        place(*legalText, 12, 98, 1156, 536);
        legalText->toFront(true);
        legal.setButtonText("CLOSE LICENSE");
    };
    addAndMakeVisible(legal);
    status.setFont(juce::FontOptions(11));
    status.setColour(juce::Label::textColourId, muted);
    addAndMakeVisible(status);
    addAndMakeVisible(keys);
    keys.setColour(juce::MidiKeyboardComponent::whiteNoteColourId, juce::Colour(0xffc9d0d3));
    keys.setColour(juce::MidiKeyboardComponent::blackNoteColourId, juce::Colour(0xff14191e));
    keys.setColour(juce::MidiKeyboardComponent::keyDownOverlayColourId, blue.withAlpha(.6f));
    keys.setAvailableRange(24, 108);
    viewport.setViewedComponent(&details, false);
    addChildComponent(viewport);
    setResizable(true, true);
    setResizeLimits(1000, 630, 1770, 1110);
    setSize(1180, 740);
    selectPage(0);
    timerCallback();
    startTimerHz(20);
}
PrismEditor::~PrismEditor()
{
    stopTimer();
    viewport.setViewedComponent(nullptr, false);
    setLookAndFeel(nullptr);
}
void PrismEditor::timerCallback()
{
    auto level = processor.peak.load();
    status.setText(juce::String(processor.voiceCount.load()) + " / " +
                       juce::String(int(processor.values[prism::polyphony]->load())) +
                       " VOICES   |   " +
                       juce::String(juce::Decibels::gainToDecibels(level, -100.f), 1) + " dBFS",
                   juce::dontSendNotification);
    auto presetName = processor.presetText("presetName");
    if (presetName.isEmpty())
        presetName = processor.getProgramName(processor.getCurrentProgram());
    programs.setButtonText(presetName + "  /  Browse");
    bool changed = !visualInitialised || level != lastPeak ||
                   lastAssetGeneration != processor.assetGeneration.load();
    lastAssetGeneration = processor.assetGeneration.load();
    lastPeak = level;
    for (size_t i = 0; i < lastValues.size(); ++i)
    {
        float v = processor.values[i]->load();
        changed = changed || v != lastValues[i];
        lastValues[i] = v;
    }
    visualInitialised = true;
    if (page == 9 && scope && changed)
        scope->repaint();
    for (auto &c : controls)
        if (c)
            c->refreshEnabled();
    details.refreshEnabled();
    if (page == 7)
        assetMessage.setText(processor.getAssetStatus(), juce::dontSendNotification);
    if (changed)
        repaint();
}
void PrismEditor::selectPage(int selected)
{
    page = selected;
    for (size_t i = 0; i < tabs.size(); ++i)
        tabs[i].setToggleState(int(i) == page, juce::dontSendNotification);
    viewport.setVisible(page != 0 && page != 5 && page != 6 && page != 9);
    if (scope)
        scope->setVisible(page == 9);
    if (page == 9 && !scope)
    {
        scope = std::make_unique<PrismScope>(processor);
        addAndMakeVisible(*scope);
    }
    for (auto &b : loadButtons)
        b.setVisible(page == 7);
    assetMessage.setVisible(page == 7);
    if (tableEditor)
        tableEditor->setVisible(page == 6);
    if (page == 6 && !tableEditor)
    {
        tableEditor = std::make_unique<PrismTableEditor>(processor);
        addAndMakeVisible(*tableEditor);
    }
    if (browser)
        browser->setVisible(page == 5);
    if (page == 5 && !browser)
    {
        browser = std::make_unique<PrismBrowser>(processor);
        addAndMakeVisible(*browser);
    }
    envelopes.setVisible(page == 0);
    std::vector<PrismDetails::Group> groups;
    auto range = [](int from, int through)
    {
        std::vector<int> ids;
        for (int i = from; i <= through; ++i)
            ids.push_back(i);
        return ids;
    };
    if (page == 1)
    {
        for (int o = 0; o < 3; ++o)
        {
            auto idx = prism::oscillatorIndices[size_t(o)];
            groups.push_back(
                {"OSC " + juce::String(o + 1) + " / TUNING, PHASE, INTERACTION AND ROUTING",
                 {idx.fine, idx.spread, idx.phase, idx.random, idx.interaction,
                  idx.interactionAmount, idx.route, int(prism::osc1_warp2 + o * 2),
                  int(prism::osc1_warp2Amount + o * 2)}});
        }
        groups.push_back({"SUB AND NOISE", range(prism::sub_level, prism::noise_pan)});
        groups.push_back({"SUB / NOISE ROUTING", {prism::sub_route, prism::noise_route}});
        groups.push_back(
            {"FILTERS / SERIAL OR PARALLEL", range(prism::filter1_type, prism::filter_mix)});
    }
    if (page == 2)
    {
        groups.push_back({"SERIAL CHAIN ORDER / DUPLICATE SLOTS ARE SKIPPED",
                          range(prism::fx_slot0, prism::fx_slot7)});
        for (int e = 0; e < 8; ++e)
        {
            int end = e == 7 ? prism::limiter_ceiling : prism::effectEnabled[size_t(e + 1)] - 1;
            groups.push_back(
                {prism::effectNames[size_t(e)], range(prism::effectEnabled[size_t(e)], end)});
        }
    }
    if (page == 3)
    {
        for (int r = 0; r < 16; ++r)
            groups.push_back({"MODULATION ROUTE " + juce::String(r + 1),
                              range(prism::mod1_source + r * 6, prism::mod1_source + r * 6 + 5)});
        for (int l = 0; l < 10; ++l)
        {
            auto ids = range(prism::lfo1_rate + l * 8, prism::lfo1_rate + l * 8 + 7);
            auto points = range(prism::lfo1_point1 + l * 8, prism::lfo1_point1 + l * 8 + 7);
            ids.insert(ids.end(), points.begin(), points.end());
            groups.push_back(
                {"LFO " + juce::String(l + 1) + " / CUSTOM SHAPE HAS EIGHT EDITABLE POINTS",
                 std::move(ids)});
        }
        for (int e = 0; e < 4; ++e)
            groups.push_back(
                {"ENVELOPE " + juce::String(e + 1) + " / STAGE CURVES",
                 range(prism::env1_attackCurve + e * 3, prism::env1_attackCurve + e * 3 + 2)});
        for (int e = 0; e < 3; ++e)
            groups.push_back({"ENVELOPE " + juce::String(e + 2),
                              range(prism::env2_attack + e * 4, prism::env2_attack + e * 4 + 3)});
    }
    if (page == 8)
    {
        groups.push_back({"ARPEGGIATOR / HOLD NOTES / SAMPLE-OFFSET INTERNAL NOTE GENERATION",
                          range(prism::arp_enabled, prism::arp_latch)});
        groups.push_back({"PATTERN SEQUENCER / RELATIVE TO LAST HELD NOTE",
                          range(prism::seq_enabled, prism::seq_latch)});
        for (int step = 0; step < 16; ++step)
            groups.push_back(
                {"STEP " + juce::String(step + 1),
                 range(prism::step1_note + step * 4, prism::step1_note + step * 4 + 3)});
    }
    if (page == 7)
    {
        groups.push_back({"BACKGROUND ASSET IMPORT / SELECT ENGINE BEFORE LOADING", {}});
        for (int o = 0; o < 3; ++o)
            groups.push_back(
                {"OSCILLATOR " + juce::String(o + 1) + " / HYBRID ENGINE AND SOURCE EDITING", [&]
                 {
                     auto ids =
                         range(prism::hybrid1_mode + o * 18, prism::hybrid1_mode + o * 18 + 17);
                     ids.push_back(int(prism::hybrid1_oneShot + o));
                     return ids;
                 }()});
    }
    if (page == 4)
    {
        groups.push_back({"MPE AND STANDALONE TEMPO", range(prism::mpe, prism::internalTempo)});
        groups.push_back({"VOICE MODE / LINEAR PORTAMENTO / FIXED RANDOM SEED",
                          range(prism::polyphony, prism::seed)});
        groups.push_back(
            {"EIGHT HOST-AUTOMATABLE PERFORMANCE MACROS", range(prism::macro1, prism::macro8)});
    }
    if (page == 5)
    {
        groups.push_back(
            {"FACTORY PROGRAMS / SELECT USING THE TOP BROWSER / USER PRESET WORKFLOW NEXT", {}});
    }
    details.setGroups(std::move(groups));
    resized();
    repaint();
}
void PrismEditor::drawWave(juce::Graphics &g, int o)
{
    float x = 25 + o * 296.f;
    juce::Rectangle<float> r(x, 144, 260, 93);
    g.setColour(juce::Colour(0xff171f21));
    g.fillRect(r);
    grid(g, r);
    int base = prism::hybrid1_mode + o * 18, mode = int(processor.values[size_t(base)]->load());
    auto pin = processor.engine.assets.pin(o);
    if (mode > 0)
    {
        if (!pin.asset || pin.asset->zones.empty())
        {
            word(g, "LOAD A SOURCE IN ASSETS", r.getX(), r.getY() + 30, r.getWidth(), 30, 11, muted,
                 juce::Justification::centred);
            return;
        }
        if (mode == 4 && !pin.asset->spectrum.empty())
        {
            auto &spectrum = pin.asset->spectrum[size_t(std::clamp(
                int(processor.values[size_t(base + 15)]->load() * (pin.asset->spectrum.size() - 1)),
                0, int(pin.asset->spectrum.size()) - 1))];
            for (int h = 0; h < 32; ++h)
            {
                float height = std::min(1.f, spectrum[size_t(h)] * 3) * r.getHeight();
                g.setColour(green);
                g.fillRect(r.getX() + h * r.getWidth() / 32, r.getBottom() - height,
                           r.getWidth() / 32 - 2, height);
            }
            return;
        }
        auto &zone = pin.asset->zones[0];
        juce::Path source;
        for (int i = 0; i < 260; ++i)
        {
            size_t index = size_t(double(i) / 259 * (zone.l.size() - 1));
            float y = r.getCentreY() - (zone.l[index] + zone.r[index]) * .25f * r.getHeight();
            if (i == 0)
                source.startNewSubPath(r.getX(), y);
            else
                source.lineTo(r.getX() + i * r.getWidth() / 259, y);
        }
        g.setColour(green);
        g.strokePath(source, juce::PathStrokeType(1));
        for (int edge = 2; edge <= 3; ++edge)
        {
            g.setColour(edge == 2 ? blue : green);
            g.drawVerticalLine(
                int(r.getX() + processor.values[size_t(base + edge)]->load() * r.getWidth()),
                r.getY(), r.getBottom());
        }
        return;
    }
    auto idx = prism::oscillatorIndices[size_t(o)];
    float frame = processor.values[size_t(idx.table)]->load() * 3;
    int first = std::min(2, int(frame));
    float mix = frame - first;
    auto shape = [](float phase, int f)
    {
        double v = 0;
        for (int k = 1; k <= 24; ++k)
        {
            double a = 0;
            if (f == 0)
                a = k == 1 ? 1 : 0;
            if (f == 1 && k % 2)
                a = ((k % 4) == 1 ? 1. : -1.) / (k * k) * 8 / (prism::pi * prism::pi);
            if (f == 2)
                a = 2 / (prism::pi * k) * (k % 2 ? 1 : -1);
            if (f == 3 && k % 2)
                a = 4 / (prism::pi * k);
            v += a * std::sin(phase * 2 * prism::pi * k);
        }
        return float(v);
    };
    for (int layer = 4; layer >= 0; --layer)
    {
        juce::Path p;
        for (int i = 0; i <= 150; ++i)
        {
            float phase = i / 150.f,
                  y = pin.asset && pin.asset->kind == prism::Asset::wavetable
                          ? pin.asset->read(5, phase, frame / 3)
                          : shape(phase, first) * (1 - mix) + shape(phase, first + 1) * mix,
                  px = r.getX() + 5 + phase * (r.getWidth() - 30) + layer * 4,
                  py = r.getCentreY() + 10 - y * 26 - layer * 4;
            if (i == 0)
                p.startNewSubPath(px, py);
            else
                p.lineTo(px, py);
        }
        g.setColour(green.withAlpha(layer == 0 ? 1.f : .2f));
        g.strokePath(p, juce::PathStrokeType(layer == 0 ? 1.6f : .7f));
    }
}
void PrismEditor::drawEnvelope(juce::Graphics &g)
{
    juce::Rectangle<float> r(25, 488, 543, 62);
    g.setColour(juce::Colour(0xff192127));
    g.fillRect(r);
    grid(g, r);
    int base = envelopeIndex == 0 ? prism::attack : prism::env2_attack + (envelopeIndex - 1) * 4;
    float a = processor.values[size_t(base)]->load(),
          d = processor.values[size_t(base + 1)]->load(),
          s = processor.values[size_t(base + 2)]->load(),
          rel = processor.values[size_t(base + 3)]->load() * s, total = a + d + .35f + rel;
    float x0 = r.getX() + 5, x1 = x0 + (r.getWidth() - 10) * a / total,
          x2 = x1 + (r.getWidth() - 10) * d / total, x3 = x2 + (r.getWidth() - 10) * .35f / total,
          bottom = r.getBottom() - 4, top = r.getY() + 5, sy = bottom - (bottom - top) * s;
    juce::Path p;
    p.startNewSubPath(x0, bottom);
    for (int i = 1; i <= 48; ++i)
    {
        float t = i / 48.f;
        p.lineTo(
            x0 + (x1 - x0) * t,
            bottom -
                (bottom - top) *
                    std::pow(t, std::exp2(
                                    processor
                                        .values[size_t(prism::env1_attackCurve + envelopeIndex * 3)]
                                        ->load() *
                                    2)));
    }
    for (int i = 1; i <= 48; ++i)
    {
        float t = i / 48.f;
        p.lineTo(
            x1 + (x2 - x1) * t,
            top +
                (sy - top) *
                    std::pow(
                        t, std::exp2(
                               processor.values[size_t(prism::env1_decayCurve + envelopeIndex * 3)]
                                   ->load() *
                               2)));
    }
    p.lineTo(x3, sy);
    for (int i = 1; i <= 48; ++i)
    {
        float t = i / 48.f;
        p.lineTo(
            x3 + (r.getRight() - 5 - x3) * t,
            sy + (bottom - sy) *
                     std::pow(
                         t,
                         std::exp2(
                             processor.values[size_t(prism::env1_releaseCurve + envelopeIndex * 3)]
                                 ->load() *
                             2)));
    }
    g.setColour(blue);
    g.strokePath(p, juce::PathStrokeType(1.8f));
}
void PrismEditor::drawFilter(juce::Graphics &g)
{
    juce::Rectangle<float> r(913, 144, 242, 93);
    g.setColour(juce::Colour(0xff192127));
    g.fillRect(r);
    grid(g, r);
    double sr = processor.getSampleRate() > 0 ? processor.getSampleRate() : 48000,
           cut = std::clamp(double(processor.values[prism::cutoff]->load()), 20., sr * .45),
           k = 2 - 1.8 * processor.values[prism::resonance]->load(),
           wc = std::tan(prism::pi * cut / sr);
    juce::Path p;
    for (int i = 0; i < 200; ++i)
    {
        double f = 20 * std::pow(sr * .48 / 20, i / 199.), w = std::tan(prism::pi * f / sr);
        std::complex<double> denominator(wc * wc - w * w, k * wc * w), numerator = wc * wc;
        int type = int(processor.values[prism::filter1_type]->load());
        if (type == 1)
            numerator = -w * w;
        else if (type == 2)
            numerator = std::complex<double>(0, wc * w);
        else if (type == 3)
            numerator = wc * wc - w * w;
        float db = float(20 * std::log10(std::max(1e-6, std::abs(numerator / denominator)))),
              x = r.getX() + i * r.getWidth() / 199,
              y = r.getY() + 20 + juce::jlimit(-12.f, 60.f, -db) * 1.05f;
        if (i == 0)
            p.startNewSubPath(x, y);
        else
            p.lineTo(x, y);
    }
    g.setColour(blue);
    g.strokePath(p, juce::PathStrokeType(1.5f));
}
void PrismEditor::paint(juce::Graphics &g)
{
    g.fillAll(background);
    g.addTransform(juce::AffineTransform::scale(getWidth() / 1180.f, getHeight() / 740.f));
    g.setColour(panel);
    g.fillRect(0, 0, 1180, 90);
    g.setColour(blue);
    g.fillRect(0, 0, 4, 90);
    juce::Path prismMark;
    prismMark.addTriangle(143.f, 18.f, 131.f, 42.f, 155.f, 42.f);
    g.strokePath(prismMark, juce::PathStrokeType(1.5f));
    g.setColour(green);
    g.drawLine(143.f, 18.f, 143.f, 42.f, 1.f);
    word(g, "PRISM", 20, 14, 150, 27, 25, text);
    word(g, "F O R G E", 21, 44, 150, 18, 13, blue);

    word(g, prism::presets[size_t(processor.getCurrentProgram())].description, 538, 47, 480, 30, 11,
         muted);
    for (int channel = 0; channel < 2; ++channel)
    {
        float value = channel ? processor.peakR.load() : processor.peakL.load(),
              height =
                  juce::jlimit(0.f, 1.f, (juce::Decibels::gainToDecibels(value, -60.f) + 60) / 60) *
                  70;
        g.setColour(juce::Colour(0xff11171c));
        g.fillRect(1162 + channel * 7, 11, 5, 70);
        g.setColour(value >= 1 ? juce::Colours::red : green);
        g.fillRect(1162.f + channel * 7, 81 - height, 5.f, height);
    }
    if (page == 0)
    {
        for (int o = 0; o < 3; ++o)
        {
            float x = 12 + o * 296.f;
            box(g, x, 98, 286, 340,
                "OSC " + juce::String::charToString(juce::juce_wchar('A' + o)) + " / " +
                    juce::String(
                        prism::choiceName(
                            size_t(prism::hybrid1_mode + o * 18),
                            int(processor.values[size_t(prism::hybrid1_mode + o * 18)]->load())))
                        .toUpperCase(),
                green);
            word(g, "SHAPE / SOURCE PREVIEW", x + 12, 125, 262, 18, 9, muted);
            drawWave(g, o);
        }
        box(g, 900, 98, 268, 340, "DUAL FILTER / TPT", blue);
        word(g, "RESPONSE / FILTER 1", 913, 125, 242, 18, 9, muted);
        drawFilter(g);
        box(g, 12, 450, 570, 184, "ENVELOPES / MODULATION SOURCES", blue);
        drawEnvelope(g);
        box(g, 594, 450, 574, 184, "PERFORMANCE MACROS / FIRST FOUR ASSIGNED IN EVERY PATCH", blue);
    }
    word(g, "MIDI -> VOICES -> OSCILLATORS -> FILTERS -> SERIAL EFFECTS", 18, 638, 620, 21, 10,
         muted);
    g.setColour(juce::Colour(0xff3c4852));
    g.drawHorizontalLine(662, 12, 1168);
    word(g, "PF", 20, 677, 49, 30, 20, blue);
    word(g, "24-108", 18, 713, 60, 15, 9, muted);
}
void PrismEditor::place(juce::Component &c, int x, int y, int w, int h)
{
    float sx = getWidth() / 1180.f, sy = getHeight() / 740.f;
    c.setBounds(juce::roundToInt(x * sx), juce::roundToInt(y * sy), juce::roundToInt(w * sx),
                juce::roundToInt(h * sy));
}
void PrismEditor::control(int id, int x, int y, int w, int h)
{
    auto &c = controls[size_t(id)];
    if (!c)
    {
        c = std::make_unique<PrismControl>(processor, size_t(id));
        addAndMakeVisible(*c);
    }
    c->setVisible(true);
    place(*c, x, y, w, h);
}
void PrismEditor::resized()
{
    for (auto &c : controls)
        if (c)
            c->setVisible(false);
    for (int i = 0; i < 14; ++i)
        place(*quickSources[size_t(i)], 181 + i * 60, 68, 58, 22);
    for (int i = 0; i < 10; ++i)
        place(tabs[size_t(i)], 159 + i * 37, 8, 36, 51);
    place(legal, 20, 68, 130, 22);
    place(programs, 538, 10, 412, 31);
    place(previous, 958, 10, 31, 31);
    place(next, 993, 10, 31, 31);
    control(prism::gain, 1043, 2, 112, 84);
    if (page == 0)
    {
        for (int o = 0; o < 3; ++o)
        {
            auto idx = prism::oscillatorIndices[size_t(o)];
            std::array<int, 8> ids{idx.table, idx.level, idx.unison, idx.detune,
                                   idx.pan,   idx.pitch, idx.warp,   idx.warpAmount};
            for (int i = 0; i < 8; ++i)
                control(ids[size_t(i)], 20 + o * 296 + (i % 4) * 68, 246 + (i / 4) * 91, 68, 84);
        }
        std::array<int, 6> filter{prism::cutoff,       prism::resonance,
                                  prism::filter1_type, prism::filter2_cutoff,
                                  prism::filter2_res,  prism::filter2_enabled};
        for (int i = 0; i < 6; ++i)
            control(filter[size_t(i)], 906 + (i % 3) * 86, 246 + (i / 3) * 91, 86, 84);
        int base =
            envelopeIndex == 0 ? prism::attack : prism::env2_attack + (envelopeIndex - 1) * 4;
        for (int i = 0; i < 4; ++i)
            control(base + i, 32 + i * 137, 554, 115, 76);
        for (int i = 0; i < 8; ++i)
            control(prism::macro1 + i, 607 + (i % 4) * 138, 478 + (i / 4) * 77, 128, 73);
        place(envelopes, 416, 453, 158, 25);
    }
    place(viewport, 12, 98, 1156, page == 7 ? 505 : 536);
    for (int o = 0; o < 3; ++o)
        place(loadButtons[size_t(o)], 778 + o * 127, 103, 121, 24);
    place(assetMessage, 16, 606, 1148, 28);
    if (browser)
        place(*browser, 12, 98, 1156, 536);
    if (tableEditor)
        place(*tableEditor, 12, 98, 1156, 536);
    if (scope)
        place(*scope, 12, 98, 1156, 536);
    details.setSize(viewport.getWidth() - 18, std::max(1, details.getHeight()));
    place(status, 710, 638, 365, 23);
    place(panic, 1088, 638, 79, 23);
    place(keys, 85, 672, 1083, 56);
    keys.setKeyWidth((keys.getWidth() - 2) / 50.f);
    if (legalText)
    {
        place(*legalText, 12, 98, 1156, 536);
        legalText->toFront(false);
    }
}

void PrismEditor::mouseDown(const juce::MouseEvent &event)
{
    if (page != 0)
        return;
    auto point = juce::Point<float>(event.position.x * 1180 / getWidth(),
                                    event.position.y * 740 / getHeight());
    for (int o = 0; o < 3; ++o)
    {
        juce::Rectangle<float> r(25 + o * 296.f, 144, 260, 93);
        int base = prism::hybrid1_mode + o * 18, mode = int(processor.values[size_t(base)]->load());
        if (r.contains(point) && mode > 0 && mode < 4)
        {
            float normalized = (point.x - r.getX()) / r.getWidth();
            waveformDrag =
                base + (std::abs(normalized - processor.values[size_t(base + 2)]->load()) <
                                std::abs(normalized - processor.values[size_t(base + 3)]->load())
                            ? 2
                            : 3);
            dragWaveX = r.getX();
            dragWaveWidth = r.getWidth();
            processor.state.getParameter(prism::parameters[size_t(waveformDrag)].id)
                ->beginChangeGesture();
            mouseDrag(event);
            break;
        }
    }
}
void PrismEditor::mouseDrag(const juce::MouseEvent &event)
{
    if (waveformDrag < 0)
        return;
    float value =
        juce::jlimit(0.f, 1.f, (event.position.x * 1180 / getWidth() - dragWaveX) / dragWaveWidth);
    auto *parameter = processor.state.getParameter(prism::parameters[size_t(waveformDrag)].id);
    parameter->setValueNotifyingHost(parameter->convertTo0to1(value));
}
void PrismEditor::mouseUp(const juce::MouseEvent &)
{
    if (waveformDrag >= 0)
        processor.state.getParameter(prism::parameters[size_t(waveformDrag)].id)
            ->endChangeGesture();
    waveformDrag = -1;
}
