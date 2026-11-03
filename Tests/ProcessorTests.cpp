#include "Plugin.h"
#include "PresetBrowser.h"
#include <cstdlib>
#include <iostream>
#include <new>
thread_local bool monitorAllocations = false;
thread_local size_t renderAllocations = 0, renderDeallocations = 0;
void *operator new(std::size_t n)
{
    if (monitorAllocations)
        ++renderAllocations;
    if (auto *p = std::malloc(n ? n : 1))
        return p;
    throw std::bad_alloc();
}
void *operator new[](std::size_t n)
{
    return ::operator new(n);
}
void operator delete(void *p) noexcept
{
    if (monitorAllocations)
        ++renderDeallocations;
    std::free(p);
}
void operator delete[](void *p) noexcept
{
    if (monitorAllocations)
        ++renderDeallocations;
    std::free(p);
}
void *operator new(std::size_t n, std::align_val_t alignment)
{
    if (monitorAllocations)
        ++renderAllocations;
    void *p = nullptr;
    if (posix_memalign(&p, size_t(alignment), n ? n : 1) == 0)
        return p;
    throw std::bad_alloc();
}
void operator delete(void *p, std::align_val_t) noexcept
{
    if (monitorAllocations)
        ++renderDeallocations;
    std::free(p);
}
void check(bool b, const char *s)
{
    if (!b)
    {
        std::cerr << s << '\n';
        std::exit(1);
    }
}
int main(int argc, char **argv)
{
    juce::ScopedJuceInitialiser_GUI init;
    auto p = std::make_unique<PrismProcessor>();
    auto q = std::make_unique<PrismProcessor>();
    for (auto *parameter : p->getParameters())
    {
        auto value = parameter->getDefaultValue();
        parameter->setValueNotifyingHost(value);
        check(parameter->getValue() == value, "exact normalized default retention");
    }
    p->setCurrentProgram(0);
    for (double sr : {44100., 48000., 96000.})
    {
        p->prepareToPlay(sr, 1024);
        for (int size : {1, 7, 64, 257, 1024, 3})
        {
            juce::AudioBuffer<float> b(2, size);
            juce::MidiBuffer m;
            m.addEvent(juce::MidiMessage::noteOn(1, 69, 1.f), size / 2);
            p->processBlock(b, m);
            if (size == 1)
                check(b.getSample(0, 0) == 0, "initial phase");
            for (int n = 0; n < size; ++n)
                check(std::isfinite(b.getSample(0, n)), "variable block finite");
            p->engine.reset();
        }
    }
    p->prepareToPlay(48000, 128);
    juce::AudioBuffer<float> b(2, 128);
    juce::MidiBuffer m;
    m.addEvent(juce::MidiMessage::noteOn(1, 69, 1.f), 63);
    p->processBlock(b, m);
    for (int i = 0; i < 63; ++i)
        check(b.getSample(0, i) == 0, "sample offset early");
    check(b.getMagnitude(0, 64, 64) > 0, "sample offset late");
    for (int i = 0; i < p->getNumPrograms(); ++i)
    {
        p->setCurrentProgram(i);
        juce::MemoryBlock saved;
        p->getStateInformation(saved);
        if (!q->restoreState(saved.getData(), int(saved.getSize())))
        {
            std::cerr << "Rejected program " << i << ": " << p->getProgramName(i) << '\n';
            auto xml =
                juce::AudioProcessor::getXmlFromBinary(saved.getData(), int(saved.getSize()));
            auto tree = juce::ValueTree::fromXml(*xml);
            for (const auto &def : prism::parameters)
            {
                double value = double(tree.getChildWithProperty("id", def.id)["value"]);
                if (value < def.min || value > def.max ||
                    (def.step > 0 && std::abs((value - def.min) / def.step -
                                              std::round((value - def.min) / def.step)) > 1e-4))
                    std::cerr << def.id << " = " << value << '\n';
            }
            check(false, "factory state rejected");
        }
        check(q->getCurrentProgram() == i, "program recall");
        for (auto &def : prism::parameters)
        {
            float a = p->state.getRawParameterValue(def.id)->load(),
                  b = q->state.getRawParameterValue(def.id)->load();
            check(std::abs(a - b) < 1e-5f * std::max(1.f, std::abs(a)), "value recall");
        }
    }
    // Decimal XML must round-trip exact float range endpoints, including 0.001f.
    for (bool maximum : {false, true})
    {
        p->setCurrentProgram(0);
        for (const auto &def : prism::parameters)
        {
            auto *parameter = p->state.getParameter(def.id);
            parameter->setValueNotifyingHost(parameter->convertTo0to1(maximum ? def.max : def.min));
        }
        juce::MemoryBlock endpoints;
        p->getStateInformation(endpoints);
        check(q->restoreState(endpoints.getData(), int(endpoints.getSize())),
              "parameter endpoint state rejected");
        for (size_t i = 0; i < prism::paramCount; ++i)
            check(std::abs(p->values[i]->load() - q->values[i]->load()) <=
                      1e-5f * std::max(1.f, std::abs(p->values[i]->load())),
                  "parameter endpoint recall");
    }
    float before = q->values[0]->load();
    const char invalid[] = "broken";
    q->setStateInformation(invalid, sizeof(invalid));
    check(q->values[0]->load() == before, "invalid state mutation");
    auto xml = p->state.copyState().createXml();
    xml->setAttribute("schema", 999);
    juce::MemoryBlock bad;
    juce::AudioProcessor::copyXmlToBinary(*xml, bad);
    q->setStateInformation(bad.getData(), int(bad.getSize()));
    check(q->values[0]->load() == before, "unknown schema mutation");
    // An old session carries only the eight foundation IDs. New parameters migrate to defaults.
    auto legacy = p->state.copyState();
    for (int i = legacy.getNumChildren() - 1; i >= 0; --i)
    {
        bool keep = false;
        for (int j = 0; j < 8; ++j)
            keep = keep || legacy.getChild(i)["id"].toString() == prism::parameters[size_t(j)].id;
        if (!keep)
            legacy.removeChild(i, nullptr);
    }
    legacy.setProperty("schema", 1, nullptr);
    auto oldXml = legacy.createXml();
    juce::MemoryBlock oldData;
    juce::AudioProcessor::copyXmlToBinary(*oldXml, oldData);
    q->setStateInformation(oldData.getData(), int(oldData.getSize()));
    for (size_t i = 8; i < prism::paramCount; ++i)
        check(std::abs(q->values[i]->load() - prism::parameters[i].def) <
                  1e-4f * std::max(1.f, std::abs(prism::parameters[i].def)),
              "v1 migration default");
    p->setCurrentProgram(0);
    q->setCurrentProgram(0);
    p->prepareToPlay(48000, 128);
    q->prepareToPlay(48000, 128);
    {
        std::unique_ptr<juce::AudioProcessorEditor> editor(p->createEditor());
        editor->setSize(1100, 650);
        juce::TextButton *browse = nullptr;
        for (auto *child : editor->getChildren())
            if (child->getName() == "Browse presets")
                browse = dynamic_cast<juce::TextButton *>(child);
        check(browse && browse->onClick, "preset browser entry button");
        browse->onClick();
        PrismBrowser *activeBrowser = nullptr;
        for (auto *child : editor->getChildren())
            if (auto *candidate = dynamic_cast<PrismBrowser *>(child))
                activeBrowser = candidate;
        check(activeBrowser && activeBrowser->isVisible(), "preset name opens real browser");
        {
            auto &browser = *activeBrowser;
            std::function<juce::Component *(juce::Component &, const juce::String &)> find =
                [&](juce::Component &root, const juce::String &name) -> juce::Component *
            {
                if (root.getName() == name)
                    return &root;
                for (auto *child : root.getChildren())
                    if (auto *found = find(*child, name))
                        return found;
                return nullptr;
            };
            auto *brass = dynamic_cast<juce::TextButton *>(find(browser, "Brass presets"));
            auto *list = dynamic_cast<juce::ListBox *>(find(browser, "Categorized presets"));
            auto *search = dynamic_cast<juce::TextEditor *>(find(browser, "Search presets"));
            check(brass && list && search, "category browser controls");
            brass->onClick();
            check(list->getListBoxModel()->getNumRows() >= 8, "Brass category shows presets");
            list->selectRow(0);
            check(p->presetText("category") == "Brass", "category selection loads matching patch");
            check(p->presetText("presetName") == "EDM Arena Brass",
                  "clicked sound changes preset name");
            check(std::abs(p->state.getRawParameterValue("cutoff")->load() - 900.f) < .01f,
                  "clicked preset changes audio parameter");
            check(list->getListBoxModel()->getNameForRow(0).contains("EDM Arena Brass"),
                  "preset row exposes its name");
            p->prepareToPlay(48000, 128);
            q->setCurrentProgram(0);
            q->prepareToPlay(48000, 128);
            juce::AudioBuffer<float> selectedAudio(2, 128), initAudio(2, 128);
            juce::MidiBuffer selectedMidi, initMidi;
            selectedMidi.addEvent(juce::MidiMessage::noteOn(1, 60, .8f), 0);
            initMidi.addEvent(juce::MidiMessage::noteOn(1, 60, .8f), 0);
            double changedAudio = 0;
            for (int block = 0; block < 100; ++block)
            {
                p->processBlock(selectedAudio, selectedMidi);
                q->processBlock(initAudio, initMidi);
                for (int n = 0; n < 128; ++n)
                    changedAudio +=
                        std::abs(selectedAudio.getSample(0, n) - initAudio.getSample(0, n));
            }
            check(changedAudio > .1, "clicked preset did not change rendered audio");
            search->setText("no-such-prism-preset-012345", false);
            search->onTextChange();
            check(list->getListBoxModel()->getNumRows() == 0, "category search empty result");
            search->clear();
            search->onTextChange();
            check(list->getListBoxModel()->getNumRows() >= 8, "category search reset");
            if (argc > 1)
            {
                for (auto *child : browser.getChildren())
                    if (auto *button = dynamic_cast<juce::TextButton *>(child))
                        if (button->getButtonText() == "Edit details")
                            button->onClick();
                browser.setSize(979, 452);
                auto image = browser.createComponentSnapshot(browser.getLocalBounds());
                auto file = juce::File::getCurrentWorkingDirectory().getChildFile(argv[1]);
                file = file.getSiblingFile(file.getFileNameWithoutExtension() +
                                           "-PATCH-details-small.png");
                if (auto out = file.createOutputStream())
                {
                    out->setPosition(0);
                    out->truncate();
                    juce::PNGImageFormat().writeImageToStream(image, *out);
                }
            }
        }
        p->setCurrentProgram(0);
        q->setCurrentProgram(0);
        p->prepareToPlay(48000, 128);
        q->prepareToPlay(48000, 128);
        if (argc > 1)
        {
            for (auto dimensions : {juce::Point<int>(1180, 740), juce::Point<int>(1000, 630),
                                    juce::Point<int>(1440, 900)})
            {
                editor->setSize(dimensions.x, dimensions.y);
                for (auto *child : editor->getChildren())
                    if (child->isVisible())
                        check(editor->getLocalBounds().contains(child->getBounds()),
                              "visible child outside resized editor");
                auto image = editor->createComponentSnapshot(editor->getLocalBounds());
                juce::File file = juce::File::getCurrentWorkingDirectory().getChildFile(argv[1]);
                if (dimensions.x != 1180)
                    file = file.getSiblingFile(file.getFileNameWithoutExtension() + "-" +
                                               juce::String(dimensions.x) + ".png");
                auto out = file.createOutputStream();
                if (out)
                {
                    out->setPosition(0);
                    out->truncate();
                    juce::PNGImageFormat().writeImageToStream(image, *out);
                }
            }
            for (auto title :
                 {"MIX", "FX", "MOD", "SETUP", "PATCH", "TABLE", "ASSET", "ARP", "SCOPE", "OSC"})
            {
                for (auto *child : editor->getChildren())
                    if (auto *button = dynamic_cast<juce::TextButton *>(child))
                        if (button->getButtonText() == title && button->onClick)
                        {
                            button->onClick();
                            break;
                        }
                editor->setSize(1180, 740);
                auto image = editor->createComponentSnapshot(editor->getLocalBounds());
                auto file = juce::File::getCurrentWorkingDirectory().getChildFile(argv[1]);
                file = file.getSiblingFile(file.getFileNameWithoutExtension() + "-" +
                                           juce::String(title) + ".png");
                if (auto out = file.createOutputStream())
                {
                    out->setPosition(0);
                    out->truncate();
                    juce::PNGImageFormat().writeImageToStream(image, *out);
                }
            }
        }
    }
    juce::AudioBuffer<float> other(2, 128);
    juce::MidiBuffer a, c;
    a.addEvent(juce::MidiMessage::noteOn(1, 60, 1.f), 0);
    c.addEvent(juce::MidiMessage::noteOn(1, 60, 1.f), 0);
    p->processBlock(b, a);
    q->processBlock(other, c);
    for (int n = 0; n < 128; ++n)
        check(b.getSample(0, n) == other.getSample(0, n), "editor changed audio");
    for (int n = 0; n < 300; ++n)
        p->enqueue(0x90, 60, 127);
    p->processBlock(b, a);
    check(p->voiceCount.load() == 0, "queue overflow recovery");
    p->setCurrentProgram(18);
    p->prepareToPlay(48000, 128);
    juce::MidiBuffer events;
    events.ensureSize(4096);
    for (int note = 48; note < 64; ++note)
        events.addEvent(juce::MidiMessage::noteOn(1, note, .8f), 0);
    monitorAllocations = true;
    for (int block = 0; block < 100; ++block)
        p->processBlock(b, events);
    monitorAllocations = false;
    check(renderAllocations == 0, "audio-thread C++ allocation");
    check(renderDeallocations == 0, "audio-thread C++ deallocation");
    for (auto name : {"Grain Aurora", "Spectral Loom", "Mallet Photon"})
    {
        for (int program = 0; program < p->getNumPrograms(); ++program)
            if (p->getProgramName(program) == name)
            {
                p->setCurrentProgram(program);
                break;
            }
        p->prepareToPlay(48000, 128);
        events.addEvent(juce::MidiMessage::noteOn(1, 60, .8f), 0);
        monitorAllocations = true;
        for (int block = 0; block < 100; ++block)
            p->processBlock(b, events);
        monitorAllocations = false;
        check(renderAllocations == 0 && renderDeallocations == 0, "hybrid render memory operation");
    }
    std::cout << "Processor offsets, variable blocks, all factory state round trips, malformed "
                 "state and multiple instances and zero C++ render allocations/deallocations in "
                 "wavetable, sample, granular and spectral playback passed.\n";
}
