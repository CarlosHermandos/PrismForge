#include "Plugin.h"
#include "Editor.h"
#include "FactoryAssets.h"
#include "StableParameter.h"
namespace
{
class AssetTask : public juce::ThreadPoolJob
{
    std::function<void()> function;

  public:
    explicit AssetTask(std::function<void()> f)
        : ThreadPoolJob("Prepare Prism asset"), function(std::move(f))
    {
    }
    JobStatus runJob() override
    {
        function();
        return jobHasFinished;
    }
};
} // namespace
PrismProcessor::~PrismProcessor()
{
    assetWorkers.removeAllJobs(true, -1);
}
juce::String PrismProcessor::getAssetStatus()
{
    juce::ScopedLock lock(assetStatusMutex);
    return assetStatus;
}
void PrismProcessor::assetResult(int oscillator, std::unique_ptr<prism::Asset> asset,
                                 const juce::String &error, unsigned request)
{
    if (request != assetRequests[size_t(oscillator)].load())
        return;
    juce::String status = error;
    if (asset)
    {
        juce::ScopedLock lock(controlMutex);
        if (request != assetRequests[size_t(oscillator)].load())
            return;
        status =
            engine.assets.publish(oscillator, std::move(asset),
                                  stateSequence.load(std::memory_order_acquire))
                ? (error.isNotEmpty() ? error
                                      : "Asset prepared / active voices crossfade in 128 samples")
                : "Asset bank busy; try Apply again";
        assetGeneration.fetch_add(1);
    }
    juce::ScopedLock lock(assetStatusMutex);
    assetStatus = status;
}
void PrismProcessor::loadHybrid(int oscillator, const juce::File &file, int mode)
{
    if (oscillator < 0 || oscillator > 2)
        return;
    if (assetWorkers.getNumJobs() >= 4)
    {
        juce::ScopedLock lock(assetStatusMutex);
        assetStatus = "Asset queue busy; wait for preparation to finish";
        return;
    }

    unsigned request = ++assetRequests[size_t(oscillator)];
    int root = int(values[size_t(prism::hybrid1_root + oscillator * 18)]->load());
    assetWorkers.addJob(new AssetTask(
                            [this, oscillator, file, mode, root, request]
                            {
                                juce::String error;
                                auto asset =
                                    mode == 2 ? prism::importSFZ(file, error)
                                              : prism::importSample(file, mode == 4, error, root);
                                assetResult(oscillator, std::move(asset), error, request);
                            }),
                        true);
}
void PrismProcessor::prepareWavetable(int oscillator, std::vector<std::vector<float>> frames,
                                      bool normalize)
{
    if (oscillator < 0 || oscillator > 2)
        return;
    if (assetWorkers.getNumJobs() >= 4)
    {
        juce::ScopedLock lock(assetStatusMutex);
        assetStatus = "Asset queue busy; wait for preparation to finish";
        return;
    }

    unsigned request = ++assetRequests[size_t(oscillator)];
    assetWorkers.addJob(
        new AssetTask(
            [this, oscillator, frames = std::move(frames), normalize, request]() mutable
            {
                auto asset = std::make_unique<prism::Asset>();
                asset->frames = std::move(frames);
                if (!asset->prepareTables(normalize))
                {
                    assetResult(oscillator, {}, "Invalid wavetable frames", request);
                    return;
                }
                assetResult(oscillator, std::move(asset), {}, request);
            }),
        true);
}
void PrismProcessor::loadWavetable(int oscillator, const juce::File &file, int frameSize,
                                   bool normalize)
{
    if (oscillator < 0 || oscillator > 2)
        return;
    if (assetWorkers.getNumJobs() >= 4)
    {
        juce::ScopedLock lock(assetStatusMutex);
        assetStatus = "Asset queue busy; wait for preparation to finish";
        return;
    }

    unsigned request = ++assetRequests[size_t(oscillator)];
    assetWorkers.addJob(new AssetTask(
                            [this, oscillator, file, frameSize, normalize, request]
                            {
                                juce::String error;
                                auto asset =
                                    prism::importWavetable(file, frameSize, normalize, error);
                                assetResult(oscillator, std::move(asset), error, request);
                            }),
                        true);
}
juce::AudioProcessorValueTreeState::ParameterLayout PrismProcessor::layout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout l;
    for (auto &p : prism::parameters)
        l.add(std::make_unique<PrismParameter>(
            juce::ParameterID(p.id, 1), p.name,
            juce::NormalisableRange<float>(p.min, p.max, p.step, p.skew), p.def,
            juce::AudioParameterFloatAttributes()
                .withLabel(p.unit)
                .withStringFromValueFunction(
                    [index = size_t(&p - prism::parameters.data())](float v, int)
                    {
                        if (auto *choice = prism::choiceName(index, int(v)))
                            return juce::String(choice);
                        return juce::String(v, prism::parameters[index].step > 0 ||
                                                       prism::parameters[index].max > 100
                                                   ? 0
                                                   : 3);
                    })
                .withValueFromStringFunction(
                    [index = size_t(&p - prism::parameters.data())](const juce::String &text)
                    {
                        auto &d = prism::parameters[index];
                        if (d.step > 0)
                            for (int v = int(d.min); v <= int(d.max) && v < 100; ++v)
                                if (auto *name = prism::choiceName(index, v))
                                    if (text.equalsIgnoreCase(name))
                                        return float(v);
                        return text.getFloatValue();
                    })));
    return l;
}
PrismProcessor::PrismProcessor()
    : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      state(*this, nullptr, "PrismForge", layout())
{
    for (size_t i = 0; i < values.size(); ++i)
        values[i] = state.getRawParameterValue(prism::parameters[i].id);
    keyboard.addListener(this);
    setCurrentProgram(0);
}
void PrismProcessor::enqueue(int status, int a, int b) noexcept
{
    auto w = writeIndex.load(std::memory_order_relaxed);
    auto r = readIndex.load(std::memory_order_acquire);
    if (w - r < queue.size())
    {
        queue[w % queue.size()] = {status, a, b};
        writeIndex.store(w + 1, std::memory_order_release);
    }
    else
        overflow.store(true, std::memory_order_release);
}
void PrismProcessor::updateSettings() noexcept
{
    auto sequence = stateSequence.load(std::memory_order_acquire);
    if (sequence & 1u)
        return;
    auto prepared = engine.settings;
    for (size_t i = 0; i < values.size(); ++i)
    {
        float value = values[i]->load(std::memory_order_relaxed);
        auto &definition = prism::parameters[i];
        prepared[i] = std::isfinite(value) ? juce::jlimit(definition.min, definition.max, value)
                                           : definition.def;
    }
    if (sequence == stateSequence.load(std::memory_order_acquire))
    {
        engine.settings = prepared;
        acceptedStateSequence = sequence;
        engine.configurePerformance();
    }
}
void PrismProcessor::prepareToPlay(double sr, int)
{
    updateSettings();
    engine.prepare(sr);
    engine.externallyClockedAssets = true;
    hadHostPosition = false;
    keyboard.reset();
    readIndex.store(writeIndex.load());
}
void PrismProcessor::processBlock(juce::AudioBuffer<float> &b, juce::MidiBuffer &midi)
{
    juce::ScopedNoDenormals guard;
    updateSettings();
    engine.tempo = engine.settings[prism::internalTempo];
    bool hasPosition = false;
    if (auto *host = getPlayHead())
        if (auto position = host->getPosition())
        {
            engine.tempo = position->getBpm().orFallback(engine.settings[prism::internalTempo]);
            engine.transportPlaying = position->getIsPlaying();
            auto sample = position->getTimeInSamples().orFallback(expectedHostSample);
            bool discontinuity = !hadHostPosition || engine.transportPlaying != lastHostPlaying ||
                                 std::abs(sample - expectedHostSample) > 2;
            engine.synchronizeTransport(
                true, engine.transportPlaying,
                position->getPpqPosition().orFallback(
                    discontinuity && position->getTimeInSamples().hasValue()
                        ? sample * engine.tempo / (60 * std::max(8000., getSampleRate()))
                        : std::numeric_limits<double>::quiet_NaN()),
                discontinuity);
            expectedHostSample = sample + b.getNumSamples();
            hadHostPosition = true;
            lastHostPlaying = engine.transportPlaying;
            hasPosition = true;
        }
    if (!hasPosition)
    {
        engine.synchronizeTransport(false, true, 0, false);
        hadHostPosition = false;
    }
    auto r = readIndex.load(std::memory_order_relaxed);
    auto w = writeIndex.load(std::memory_order_acquire);
    if (overflow.exchange(false, std::memory_order_acq_rel))
    {
        engine.reset();
        r = w;
    }
    while (r != w)
    {
        auto e = queue[r % queue.size()];
        engine.midi(e.s, e.a, e.b);
        ++r;
    }
    readIndex.store(r, std::memory_order_release);
    engine.assets.tick(acceptedStateSequence, true, false);
    auto it = midi.cbegin();
    auto end = midi.cend();
    float maximum = 0, leftPeak = 0, rightPeak = 0;
    for (int n = 0; n < b.getNumSamples(); ++n)
    {
        while (it != end && (*it).samplePosition <= n)
        {
            auto meta = *it;
            if (meta.numBytes >= 1)
            {
                auto *d = meta.data;
                engine.midi(d[0], meta.numBytes > 1 ? d[1] : 0, meta.numBytes > 2 ? d[2] : 0);
            }
            ++it;
        }
        auto x = engine.tickStereo();
        scopeData[scopeCursor++ % scopeData.size()].store((x.l + x.r) * .5f,
                                                          std::memory_order_relaxed);
        leftPeak = std::max(leftPeak, std::abs(x.l));
        rightPeak = std::max(rightPeak, std::abs(x.r));
        maximum = std::max({maximum, std::abs(x.l), std::abs(x.r)});
        if (b.getNumChannels() > 0)
            b.setSample(0, n, x.l);
        if (b.getNumChannels() > 1)
            b.setSample(1, n, x.r);
    }
    scopeIndex.store(scopeCursor, std::memory_order_release);
    peakL.store(leftPeak, std::memory_order_relaxed);
    peakR.store(rightPeak, std::memory_order_relaxed);
    peak.store(maximum, std::memory_order_relaxed);
    voiceCount.store(engine.active(), std::memory_order_relaxed);
    midi.clear();
}
void PrismProcessor::setCurrentProgram(int i)
{
    if (i < 0 || i >= getNumPrograms())
        return;
    juce::ScopedLock lock(controlMutex);
    stateSequence.fetch_add(1, std::memory_order_acq_rel);
    for (int o = 0; o < 3; ++o)
    {
        ++assetRequests[size_t(o)];
        int mode = int(prism::presets[size_t(i)].values[size_t(prism::hybrid1_mode + o * 18)]);
        engine.assets.publish(o, mode ? prism::makeFactoryAsset(mode, i) : nullptr,
                              stateSequence.load() + 1);
    }
    assetGeneration.fetch_add(1);
    for (size_t j = 0; j < values.size(); ++j)
    {
        auto *p = state.getParameter(prism::parameters[j].id);
        p->setValueNotifyingHost(p->convertTo0to1(prism::presets[size_t(i)].values[j]));
    }
    state.state.setProperty("presetName", prism::presets[size_t(i)].name, nullptr);
    state.state.setProperty("category", prism::presets[size_t(i)].category, nullptr);
    state.state.setProperty("tags", prism::presets[size_t(i)].tags, nullptr);
    state.state.setProperty("author", prism::presets[size_t(i)].author, nullptr);
    state.state.setProperty("description", prism::presets[size_t(i)].description, nullptr);
    for (int m = 0; m < 4; ++m)
        state.state.setProperty("macroName" + juce::String(m + 1),
                                prism::presets[size_t(i)].macroNames[size_t(m)], nullptr);
    program.store(i);
    stateSequence.fetch_add(1, std::memory_order_release);
}
juce::String PrismProcessor::presetText(const juce::Identifier &key)
{
    juce::ScopedLock lock(controlMutex);
    return state.state.getProperty(key).toString();
}
void PrismProcessor::setMacroNames(const std::array<juce::String, 4> &names)
{
    juce::ScopedLock lock(controlMutex);
    for (int i = 0; i < 4; ++i)
        state.state.setProperty("macroName" + juce::String(i + 1), names[size_t(i)], nullptr);
}
void PrismProcessor::setPresetMetadata(const juce::String &name, const juce::String &category,
                                       const juce::String &tags, const juce::String &author,
                                       const juce::String &description)
{
    juce::ScopedLock lock(controlMutex);
    state.state.setProperty("presetName", name, nullptr);
    state.state.setProperty("category", category, nullptr);
    state.state.setProperty("tags", tags, nullptr);
    state.state.setProperty("author", author, nullptr);
    state.state.setProperty("description", description, nullptr);
}
void PrismProcessor::getStateInformation(juce::MemoryBlock &b)
{
    juce::ScopedLock lock(controlMutex);
    auto tree = state.copyState();
    for (int i = tree.getNumChildren() - 1; i >= 0; --i)
        if (tree.getChild(i).hasType("ASSET"))
            tree.removeChild(i, nullptr);
    for (int o = 0; o < 3; ++o)
    {
        auto pin = engine.assets.pin(o);
        if (pin.asset)
        {
            juce::ValueTree asset("ASSET");
            asset.setProperty("slot", o, nullptr);
            asset.setProperty("payload", prism::encodeAsset(*pin.asset), nullptr);
            tree.addChild(asset, -1, nullptr);
        }
    }
    tree.setProperty("schema", 2, nullptr);
    tree.setProperty("program", program.load(), nullptr);
    auto xml = tree.createXml();
    copyXmlToBinary(*xml, b);
}
void PrismProcessor::setStateInformation(const void *data, int bytes)
{
    restoreState(data, bytes);
}
bool PrismProcessor::restoreState(const void *data, int bytes)
{
    if (bytes < 1 || bytes > 64 * 1024 * 1024)
        return false;
    auto xml = getXmlFromBinary(data, bytes);
    if (!xml || !xml->hasTagName("PrismForge"))
        return false;
    int schema = xml->getIntAttribute("schema", 0);
    if (schema != 1 && schema != 2)
        return false;
    auto tree = juce::ValueTree::fromXml(*xml);
    if (!tree.isValid())
        return false;
    std::array<bool, prism::paramCount> seen{};
    for (auto child : tree)
        if (child.hasType("PARAM"))
        {
            auto id = child["id"].toString();
            for (size_t j = 0; j < prism::paramCount; ++j)
                if (id == prism::parameters[j].id)
                {
                    if (seen[j])
                        return false;
                    seen[j] = true;
                }
        }
    for (size_t i = 0; i < prism::paramCount; ++i)
    {
        auto &def = prism::parameters[i];
        auto child = tree.getChildWithProperty("id", def.id);
        if (!child.isValid() && schema == 1 && i >= 8)
        {
            child = juce::ValueTree("PARAM");
            child.setProperty("id", def.id, nullptr);
            child.setProperty("value", def.def, nullptr);
            tree.addChild(child, -1, nullptr);
        }
        if (!child.isValid() || !child.hasProperty("value"))
            return false;
        auto text = child["value"].toString();
        if (text.isEmpty() || !text.containsOnly("0123456789.+-eE"))
            return false;
        char *end = nullptr;
        auto *start = text.toRawUTF8();
        double v = std::strtod(start, &end);
        // XML uses decimal text; compare bounds at the parameter's float precision.
        // For example 0.001 as double is slightly below the float minimum 0.001f.
        const float parameterValue = static_cast<float>(v);
        if (end == start || *end != 0 || !std::isfinite(v) || !std::isfinite(parameterValue) ||
            parameterValue < def.min || parameterValue > def.max)
            return false;
        if (def.step > 0 &&
            std::abs((v - def.min) / def.step - std::round((v - def.min) / def.step)) > 1e-4)
            return false;
    }
    std::array<std::unique_ptr<prism::Asset>, 3> preparedAssets;
    std::array<bool, 3> present{};
    for (auto child : tree)
        if (child.hasType("ASSET"))
        {
            int slot = int(child["slot"]);
            if (slot < 0 || slot > 2 || present[size_t(slot)])
                return false;
            juce::String error;
            preparedAssets[size_t(slot)] = prism::decodeAsset(child["payload"].toString(), error);
            if (!preparedAssets[size_t(slot)])
                return false;
            present[size_t(slot)] = true;
        }
    tree.setProperty("schema", 2, nullptr);
    juce::ScopedLock lock(controlMutex);
    stateSequence.fetch_add(1, std::memory_order_acq_rel);
    for (int o = 0; o < 3; ++o)
    {
        ++assetRequests[size_t(o)];
        if (!engine.assets.publish(o, std::move(preparedAssets[size_t(o)]),
                                   stateSequence.load() + 1))
        {
            stateSequence.fetch_add(1, std::memory_order_release);
            return false;
        }
    }
    assetGeneration.fetch_add(1);
    state.replaceState(tree);
    program.store(juce::jlimit(0, getNumPrograms() - 1, int(tree["program"])));
    stateSequence.fetch_add(1, std::memory_order_release);
    return true;
}
juce::AudioProcessorEditor *PrismProcessor::createEditor()
{
    return new PrismEditor(*this);
}
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
    return new PrismProcessor;
}
