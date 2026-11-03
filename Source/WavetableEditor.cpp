#include "WavetableEditor.h"
PrismTableEditor::PrismTableEditor(PrismProcessor &p) : processor(p)
{
    for (int i = 0; i < 3; ++i)
        oscillator.addItem("Oscillator " + juce::String::charToString(juce::juce_wchar('A' + i)),
                           i + 1);
    oscillator.setSelectedId(1);
    oscillator.onChange = [this] { sync(); };
    addAndMakeVisible(oscillator);
    addAndMakeVisible(frameSelect);
    frameSelect.onChange = [this]
    {
        if (updating)
            return;
        selected = frameSelect.getSelectedId() - 1;
        analyse();
        repaint();
    };
    for (int n : {256, 512, 1024, 2048, 4096})
        frameSize.addItem(juce::String(n), n);
    frameSize.setSelectedId(2048);
    addAndMakeVisible(frameSize);
    addAndMakeVisible(normalize);
    normalize.setToggleState(true, juce::dontSendNotification);
    const std::array<const char *, 13> names{
        "Insert",  "Duplicate", "Delete", "Move left", "Move right", "Interpolate", "Invert",
        "Reverse", "DC remove", "Undo",   "Redo",      "Import WAV", "Export WAV"};
    for (size_t i = 0; i < buttons.size(); ++i)
    {
        buttons[i].setButtonText(names[i]);
        addAndMakeVisible(buttons[i]);
        buttons[i].onClick = [this, i]
        {
            if (i == 9)
            {
                if (undoPosition > 0)
                {
                    --undoPosition;
                    frames = undo[undoPosition];
                    changed();
                }
                return;
            }
            if (i == 10)
            {
                if (undoPosition + 1 < undo.size())
                {
                    frames = undo[++undoPosition];
                    changed();
                }
                return;
            }
            if (i == 11)
            {
                importTable();
                return;
            }
            if (i == 12)
            {
                exportTable();
                return;
            }
            if (frames.empty())
                return;
            remember();
            if (i == 0 && frames.size() < 64)
                frames.insert(frames.begin() + selected + 1, std::vector<float>(2048, 0));
            if (i == 1 && frames.size() < 64)
                frames.insert(frames.begin() + selected + 1, frames[size_t(selected)]);
            if (i == 2 && frames.size() > 1)
                frames.erase(frames.begin() + selected);
            if (i == 3 && selected > 0)
            {
                std::swap(frames[size_t(selected)], frames[size_t(selected - 1)]);
                --selected;
            }
            if (i == 4 && selected + 1 < int(frames.size()))
            {
                std::swap(frames[size_t(selected)], frames[size_t(selected + 1)]);
                ++selected;
            }
            if (i == 5 && frames.size() < 64 && selected + 1 < int(frames.size()))
            {
                auto frame = frames[size_t(selected)];
                for (size_t n = 0; n < frame.size(); ++n)
                    frame[n] = (frame[n] + frames[size_t(selected + 1)][n]) * .5f;
                frames.insert(frames.begin() + selected + 1, std::move(frame));
            }
            if (i == 6)
                for (auto &v : frames[size_t(selected)])
                    v = -v;
            if (i == 7)
                std::reverse(frames[size_t(selected)].begin(), frames[size_t(selected)].end());
            if (i == 8)
            {
                double dc = 0;
                for (auto v : frames[size_t(selected)])
                    dc += v / frames[size_t(selected)].size();
                for (auto &v : frames[size_t(selected)])
                    v -= float(dc);
            }
            changed();
        };
    }
    for (size_t i = 0; i < harmonics.size(); ++i)
    {
        auto &slider = harmonics[i];
        slider.setSliderStyle(juce::Slider::LinearVertical);
        slider.setVelocityModeParameters(0.5, 1, 0.0, true, juce::ModifierKeys::shiftModifier);
        slider.setRange(0, 1, .001);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 52, 18);
        slider.setName("Harmonic " + juce::String(int(i) + 1));
        slider.onDragStart = [this]
        {
            harmonicGesture = true;
            remember();
        };
        slider.onValueChange = [this]
        {
            if (updating || frames.empty())
                return;
            auto &frame = frames[size_t(selected)];
            for (size_t n = 0; n < frame.size(); ++n)
            {
                double v = 0;
                for (size_t h = 0; h < harmonics.size(); ++h)
                    v += harmonics[h].getValue() *
                         std::sin(2 * prism::pi * (h + 1) * n / frame.size() + phases[h]);
                frame[n] = float(v);
            }
            repaint();
            if (!harmonicGesture)
                changed();
        };
        slider.onDragEnd = [this]
        {
            harmonicGesture = false;
            changed();
        };
        addAndMakeVisible(slider);
    }
    apply.onClick = [this]
    {
        processor.prepareWavetable(oscillator.getSelectedId() - 1, frames,
                                   normalize.getToggleState());
    };
    addAndMakeVisible(apply);
    addAndMakeVisible(status);
    sync();
    startTimerHz(5);
}
PrismTableEditor::~PrismTableEditor()
{
    stopTimer();
}
void PrismTableEditor::sync()
{
    auto pin = processor.engine.assets.pin(oscillator.getSelectedId() - 1);
    if (pin.asset && pin.asset->kind == prism::Asset::wavetable)
        frames = pin.asset->frames;
    else
        frames = processor.engine.basicFrames();
    selected = 0;
    undo = {frames};
    undoPosition = 0;
    changed();
    generation = processor.assetGeneration.load();
}
void PrismTableEditor::remember()
{
    if (undo.empty() || undo[undoPosition] != frames)
    {
        undo.resize(undoPosition + 1);
        undo.push_back(frames);
        undoPosition = undo.size() - 1;
    }
    if (undo.size() > 32)
    {
        undo.erase(undo.begin());
        --undoPosition;
    }
}
void PrismTableEditor::changed()
{
    selected = juce::jlimit(0, int(frames.size()) - 1, selected);
    updating = true;
    frameSelect.clear();
    for (size_t i = 0; i < frames.size(); ++i)
        frameSelect.addItem("Frame " + juce::String(int(i) + 1), int(i) + 1);
    frameSelect.setSelectedId(selected + 1, juce::dontSendNotification);
    updating = false;
    analyse();
    repaint();
    if (!undo.empty() && undo[undoPosition] != frames)
    {
        undo.resize(undoPosition + 1);
        undo.push_back(frames);
        undoPosition = undo.size() - 1;
    }
    if (undo.size() > 32)
    {
        undo.erase(undo.begin());
        --undoPosition;
    }
    buttons[9].setEnabled(undoPosition > 0);
    buttons[10].setEnabled(undoPosition + 1 < undo.size());
    buttons[0].setEnabled(frames.size() < 64);
    buttons[1].setEnabled(frames.size() < 64);
    buttons[2].setEnabled(frames.size() > 1);
    buttons[3].setEnabled(selected > 0);
    buttons[4].setEnabled(selected + 1 < int(frames.size()));
    buttons[5].setEnabled(selected + 1 < int(frames.size()) && frames.size() < 64);
}
void PrismTableEditor::analyse()
{
    if (frames.empty())
        return;
    updating = true;
    auto &frame = frames[size_t(selected)];
    for (size_t h = 0; h < 16; ++h)
    {
        double re = 0, im = 0;
        for (size_t n = 0; n < frame.size(); ++n)
        {
            double angle = 2 * prism::pi * (h + 1) * n / frame.size();
            re += frame[n] * std::cos(angle) * 2 / frame.size();
            im += frame[n] * std::sin(angle) * 2 / frame.size();
        }
        harmonics[h].setValue(std::sqrt(re * re + im * im), juce::dontSendNotification);
        phases[h] = float(std::atan2(re, im));
    }
    updating = false;
}
void PrismTableEditor::timerCallback()
{
    if (generation != processor.assetGeneration.load() && !drawing)
        sync();
    status.setText(processor.getAssetStatus(), juce::dontSendNotification);
}
juce::Rectangle<float> PrismTableEditor::waveformBounds() const
{
    return {18, 91, float(getWidth() - 36), float(getHeight() * .36)};
}
void PrismTableEditor::drawAt(juce::Point<float> point)
{
    auto r = waveformBounds();
    if (frames.empty())
        return;
    auto &frame = frames[size_t(selected)];
    int from = juce::jlimit(0, int(frame.size()) - 1,
                            int((previous.x - r.getX()) / r.getWidth() * (frame.size() - 1))),
        to = juce::jlimit(0, int(frame.size()) - 1,
                          int((point.x - r.getX()) / r.getWidth() * (frame.size() - 1)));
    float a = juce::jlimit(-1.f, 1.f, (r.getCentreY() - previous.y) / (r.getHeight() * .45f)),
          b = juce::jlimit(-1.f, 1.f, (r.getCentreY() - point.y) / (r.getHeight() * .45f));
    int n = std::max(1, std::abs(to - from));
    for (int i = 0; i <= n; ++i)
    {
        int index = from + (to - from) * i / n;
        frame[size_t(index)] = a + (b - a) * i / n;
    }
    previous = point;
    repaint();
}
void PrismTableEditor::mouseDown(const juce::MouseEvent &e)
{
    if (waveformBounds().contains(e.position))
    {
        remember();
        drawing = true;
        previous = e.position;
        drawAt(e.position);
    }
}
void PrismTableEditor::mouseDrag(const juce::MouseEvent &e)
{
    if (drawing)
        drawAt(e.position);
}
void PrismTableEditor::mouseUp(const juce::MouseEvent &)
{
    if (drawing)
    {
        drawing = false;
        changed();
    }
}
void PrismTableEditor::importTable()
{
    chooser = std::make_unique<juce::FileChooser>("Import packed wavetable", juce::File{},
                                                  "*.wav;*.aif;*.aiff");
    juce::Component::SafePointer<PrismTableEditor> safe(this);
    chooser->launchAsync(
        juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [safe](const juce::FileChooser &c)
        {
            if (safe && c.getResult().existsAsFile())
                safe->processor.loadWavetable(safe->oscillator.getSelectedId() - 1, c.getResult(),
                                              safe->frameSize.getSelectedId(),
                                              safe->normalize.getToggleState());
        });
}
void PrismTableEditor::exportTable()
{
    chooser = std::make_unique<juce::FileChooser>(
        "Export 2048-sample frames",
        juce::File::getSpecialLocation(juce::File::userMusicDirectory)
            .getChildFile("Prism Table.wav"),
        "*.wav");
    juce::Component::SafePointer<PrismTableEditor> safe(this);
    chooser->launchAsync(
        juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles |
            juce::FileBrowserComponent::warnAboutOverwriting,
        [safe](const juce::FileChooser &c)
        {
            if (!safe || c.getResult() == juce::File{})
                return;
            auto file = c.getResult().withFileExtension("wav");
            auto out = file.createOutputStream();
            if (!out)
                return;
            out->setPosition(0);
            out->truncate();
            juce::WavAudioFormat format;
            std::unique_ptr<juce::OutputStream> output = std::move(out);
            auto writer = format.createWriterFor(output, juce::AudioFormatWriterOptions()
                                                             .withSampleRate(44100)
                                                             .withNumChannels(1)
                                                             .withBitsPerSample(24));
            if (!writer)
                return;
            juce::AudioBuffer<float> buffer(1, int(safe->frames.size() * 2048));
            for (size_t f = 0; f < safe->frames.size(); ++f)
                for (int i = 0; i < 2048; ++i)
                    buffer.setSample(0, int(f * 2048) + i,
                                     std::clamp(safe->frames[f][size_t(i)], -1.f, 1.f));
            writer->writeFromAudioSampleBuffer(buffer, 0, buffer.getNumSamples());
        });
}
void PrismTableEditor::paint(juce::Graphics &g)
{
    g.fillAll(juce::Colour(0xff20272e));
    auto r = waveformBounds();
    g.setColour(juce::Colour(0xff161f23));
    g.fillRect(r);
    g.setColour(juce::Colour(0xff35434b));
    for (int i = 1; i < 16; ++i)
        g.drawVerticalLine(int(r.getX() + i * r.getWidth() / 16), r.getY(), r.getBottom());
    g.drawHorizontalLine(int(r.getCentreY()), r.getX(), r.getRight());
    if (!frames.empty())
    {
        juce::Path p;
        auto &frame = frames[size_t(selected)];
        for (size_t i = 0; i < frame.size(); ++i)
        {
            float x = r.getX() + float(i) * r.getWidth() / (frame.size() - 1),
                  y = r.getCentreY() - std::clamp(frame[i], -1.f, 1.f) * r.getHeight() * .45f;
            if (!i)
                p.startNewSubPath(x, y);
            else
                p.lineTo(x, y);
        }
        g.setColour(juce::Colour(0xffb4dc6b));
        g.strokePath(p, juce::PathStrokeType(1.6f));
    }
    g.setColour(juce::Colour(0xffa7bac8));
    g.setFont(juce::FontOptions(10));
    for (int h = 0; h < 16; ++h)
        g.drawText(juce::String(h + 1), 18 + h * (getWidth() - 36) / 16, int(r.getBottom()) + 1,
                   (getWidth() - 36) / 16 - 3, 12, juce::Justification::centred);
    g.setFont(juce::FontOptions(11));
    g.drawText("DRAW A CYCLE / HARMONIC AMPLITUDES / APPLY PREPARES BAND-LIMITED TABLES OFF THE "
               "AUDIO THREAD",
               18, 68, getWidth() - 36, 20, juce::Justification::centredLeft);
    g.drawText("EDITING IS NON-DESTRUCTIVE UNTIL APPLY / 2048 SAMPLES PER FRAME / 64 FRAMES MAX",
               18, getHeight() - 64, getWidth() - 36, 18, juce::Justification::centredLeft);
}
void PrismTableEditor::resized()
{
    oscillator.setBounds(18, 12, 150, 28);
    frameSelect.setBounds(180, 12, 140, 28);
    frameSize.setBounds(332, 12, 95, 28);
    normalize.setBounds(440, 12, 170, 28);
    for (size_t i = 0; i < buttons.size(); ++i)
        buttons[i].setBounds(18 + int(i) * ((getWidth() - 36) / 13), 46, (getWidth() - 36) / 13 - 3,
                             22);
    int top = int(waveformBounds().getBottom()) + 12;
    for (int h = 0; h < 16; ++h)
        harmonics[size_t(h)].setBounds(18 + h * (getWidth() - 36) / 16, top,
                                       (getWidth() - 36) / 16 - 3, getHeight() - top - 74);
    status.setBounds(18, getHeight() - 39, getWidth() - 220, 28);
    apply.setBounds(getWidth() - 195, getHeight() - 39, 177, 28);
}
