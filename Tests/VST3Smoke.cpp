#include "pluginterfaces/base/ibstream.h"
#include "pluginterfaces/vst/ivstaudioprocessor.h"
#include "pluginterfaces/vst/ivstcomponent.h"
#include "pluginterfaces/vst/ivstevents.h"
#include "pluginterfaces/vst/ivsthostapplication.h"
#include <CoreFoundation/CoreFoundation.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
using namespace Steinberg;
using namespace Steinberg::Vst;
void require(bool b, const char *text)
{
    if (!b)
    {
        std::fprintf(stderr, "%s\n", text);
        std::exit(1);
    }
}
struct Host : IHostApplication
{
    tresult PLUGIN_API queryInterface(const TUID id, void **output) override
    {
        if (!std::memcmp(id, INLINE_UID_OF(IHostApplication), 16) ||
            !std::memcmp(id, INLINE_UID_OF(FUnknown), 16))
        {
            *output = this;
            return kResultOk;
        }
        *output = nullptr;
        return kNoInterface;
    }
    uint32 PLUGIN_API addRef() override
    {
        return 1;
    }
    uint32 PLUGIN_API release() override
    {
        return 1;
    }
    tresult PLUGIN_API getName(String128 name) override
    {
        const char *text = "Prism Offline VST3 Host";
        int i = 0;
        for (; text[i]; ++i)
            name[i] = char16(text[i]);
        name[i] = 0;
        return kResultOk;
    }
    tresult PLUGIN_API createInstance(TUID, TUID, void **output) override
    {
        *output = nullptr;
        return kNoInterface;
    }
};
struct Events : IEventList
{
    std::array<Event, 2> events{};
    int count = 0;
    tresult PLUGIN_API queryInterface(const TUID, void **output) override
    {
        *output = nullptr;
        return kNoInterface;
    }
    uint32 PLUGIN_API addRef() override
    {
        return 1;
    }
    uint32 PLUGIN_API release() override
    {
        return 1;
    }
    int32 PLUGIN_API getEventCount() override
    {
        return count;
    }
    tresult PLUGIN_API getEvent(int32 i, Event &e) override
    {
        if (i < 0 || i >= count)
            return kInvalidArgument;
        e = events[size_t(i)];
        return kResultOk;
    }
    tresult PLUGIN_API addEvent(Event &e) override
    {
        if (count >= 2)
            return kResultFalse;
        events[size_t(count++)] = e;
        return kResultOk;
    }
};
struct Stream : IBStream
{
    std::vector<char> bytes;
    int64 position = 0;
    tresult PLUGIN_API queryInterface(const TUID, void **output) override
    {
        *output = nullptr;
        return kNoInterface;
    }
    uint32 PLUGIN_API addRef() override
    {
        return 1;
    }
    uint32 PLUGIN_API release() override
    {
        return 1;
    }
    tresult PLUGIN_API read(void *output, int32 n, int32 *actual) override
    {
        int count = int(std::min<int64>(n, int64(bytes.size()) - position));
        if (count < 0)
            return kInvalidArgument;
        if (count)
            std::memcpy(output, bytes.data() + position, size_t(count));
        position += count;
        if (actual)
            *actual = count;
        return count == n ? kResultOk : kResultFalse;
    }
    tresult PLUGIN_API write(void *input, int32 n, int32 *actual) override
    {
        if (n < 0 || position + n > 64 * 1024 * 1024)
            return kInvalidArgument;
        bytes.resize(size_t(std::max<int64>(bytes.size(), position + n)));
        std::memcpy(bytes.data() + position, input, size_t(n));
        position += n;
        if (actual)
            *actual = n;
        return kResultOk;
    }
    tresult PLUGIN_API seek(int64 value, int32 mode, int64 *result) override
    {
        position = mode == kIBSeekSet   ? value
                   : mode == kIBSeekCur ? position + value
                                        : int64(bytes.size()) + value;
        if (position < 0)
            return kInvalidArgument;
        if (result)
            *result = position;
        return kResultOk;
    }
    tresult PLUGIN_API tell(int64 *result) override
    {
        *result = position;
        return kResultOk;
    }
};
int main(int argc, char **argv)
{
    require(argc == 2, "Pass VST3 bundle path");
    auto url = CFURLCreateFromFileSystemRepresentation(nullptr, (const UInt8 *)argv[1],
                                                       std::strlen(argv[1]), true);
    auto bundle = CFBundleCreate(nullptr, url);
    require(bundle && CFBundleLoadExecutable(bundle), "VST3 bundle load");
    auto entry =
        (bool (*)(CFBundleRef))CFBundleGetFunctionPointerForName(bundle, CFSTR("bundleEntry"));
    auto exit = (bool (*)())CFBundleGetFunctionPointerForName(bundle, CFSTR("bundleExit"));
    auto getFactory = (IPluginFactory * (*)())
        CFBundleGetFunctionPointerForName(bundle, CFSTR("GetPluginFactory"));
    require(entry && exit && getFactory && entry(bundle), "VST3 module entry");
    auto *factory = getFactory();
    require(factory, "VST3 factory");
    IComponent *component = nullptr;
    for (int32 i = 0; i < factory->countClasses(); ++i)
    {
        PClassInfo info{};
        if (factory->getClassInfo(i, &info) == kResultOk &&
            !std::strcmp(info.category, kVstAudioEffectClass))
        {
            require(factory->createInstance(info.cid, INLINE_UID_OF(IComponent),
                                            (void **)&component) == kResultOk,
                    "VST3 component creation");
            break;
        }
    }
    require(component, "VST3 audio class");
    Host host;
    require(component->initialize(&host) == kResultOk, "VST3 initialize");
    IAudioProcessor *audio = nullptr;
    require(component->queryInterface(INLINE_UID_OF(IAudioProcessor), (void **)&audio) == kResultOk,
            "VST3 audio processor");
    SpeakerArrangement stereo = 3;
    require(audio->setBusArrangements(nullptr, 0, &stereo, 1) == kResultOk, "VST3 stereo bus");
    component->activateBus(kAudio, kOutput, 0, true);
    component->activateBus(kEvent, kInput, 0, true);
    ProcessSetup setup{kRealtime, kSample32, 128, 48000};
    require(audio->setupProcessing(setup) == kResultOk, "VST3 processing setup");
    require(component->setActive(true) == kResultOk && audio->setProcessing(true) == kResultOk,
            "VST3 activate");
    float left[128]{}, right[128]{};
    float *channels[]{left, right};
    AudioBusBuffers output{};
    output.numChannels = 2;
    output.channelBuffers32 = channels;
    Events events;
    Event note{};
    note.type = Event::kNoteOnEvent;
    note.sampleOffset = 32;
    note.noteOn.channel = 0;
    note.noteOn.pitch = 69;
    note.noteOn.velocity = .8f;
    note.noteOn.noteId = 69;
    events.addEvent(note);
    ProcessData data{};
    data.processMode = kRealtime;
    data.symbolicSampleSize = kSample32;
    data.numSamples = 128;
    data.numOutputs = 1;
    data.outputs = &output;
    data.inputEvents = &events;
    float peak = 0;
    for (int block = 0; block < 2000; ++block)
    {
        if (block == 1)
            events.count = 0;
        if (block == 200)
        {
            note.type = Event::kNoteOffEvent;
            note.sampleOffset = 0;
            note.noteOff.channel = 0;
            note.noteOff.pitch = 69;
            note.noteOff.velocity = 0;
            note.noteOff.noteId = 69;
            events.addEvent(note);
        }
        if (block == 201)
            events.count = 0;
        require(audio->process(data) == kResultOk, "VST3 process");
        for (int n = 0; n < 128; ++n)
        {
            require(std::isfinite(left[n]) && std::isfinite(right[n]), "VST3 finite output");
            if (block == 0 && n < 32)
                require(left[n] == 0, "VST3 MIDI offset");
            peak = std::max(peak, std::abs(left[n]));
        }
    }
    require(peak > .001 && peak < 1 && std::abs(left[127]) < 1e-8, "VST3 sound and release");
    Stream state;
    require(component->getState(&state) == kResultOk && !state.bytes.empty(), "VST3 get state");
    state.position = 0;
    require(component->setState(&state) == kResultOk, "VST3 set state");
    audio->setProcessing(false);
    component->setActive(false);
    audio->release();
    component->terminate();
    component->release();
    factory->release();
    exit();
    CFRelease(bundle);
    CFRelease(url);
    std::printf("VST3 module/factory, stereo/MIDI offset rendering, release and state APIs passed; "
                "peak=%f\n",
                peak);
}
