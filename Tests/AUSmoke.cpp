#include "Parameters.h"
#include <algorithm>

#include <AudioToolbox/AudioToolbox.h>
#include <CoreFoundation/CoreFoundation.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
void check(bool b, const char *s)
{
    if (!b)
    {
        std::fprintf(stderr, "%s\n", s);
        std::exit(1);
    }
}
int main(int argc, char **argv)
{
    check(argc == 2, "Pass AU path");
    auto url = CFURLCreateFromFileSystemRepresentation(nullptr, (const UInt8 *)argv[1],
                                                       std::strlen(argv[1]), true);
    auto bundle = CFBundleCreate(nullptr, url);
    check(bundle && CFBundleLoadExecutable(bundle), "load bundle");
    auto factory = (AudioComponentFactoryFunction)CFBundleGetFunctionPointerForName(
        bundle, CFSTR("Prism_ForgeAUFactory"));
    check(factory, "factory");
    AudioComponentDescription desc{kAudioUnitType_MusicDevice, 'PrF1', 'PrFg', 0, 0};
    auto comp = AudioComponentRegister(&desc, CFSTR("Prism Forge local smoke"), 0x301, factory);
    AudioUnit unit = nullptr;
    auto status = AudioComponentInstanceNew(comp, &unit);
    std::fprintf(stderr, "AU instance status: %d\n", int(status));
    check(status == noErr, "instance");
    AudioStreamBasicDescription fmt{48000,
                                    kAudioFormatLinearPCM,
                                    kAudioFormatFlagsNativeFloatPacked |
                                        kAudioFormatFlagIsNonInterleaved,
                                    4,
                                    1,
                                    4,
                                    2,
                                    32,
                                    0};
    check(AudioUnitSetProperty(unit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Output, 0,
                               &fmt, sizeof(fmt)) == noErr,
          "format");
    check(AudioUnitInitialize(unit) == noErr, "initialize");
    check(MusicDeviceMIDIEvent(unit, 0x90, 69, 100, 32) == noErr, "note on");
    struct
    {
        UInt32 count = 2;
        AudioBuffer data[2];
    } buffers;
    float left[128], right[128];
    buffers.data[0] = {1, sizeof(left), left};
    buffers.data[1] = {1, sizeof(right), right};
    float peak = 0;
    for (int block = 0; block < 2000; ++block)
    {
        if (block == 200)
            check(MusicDeviceMIDIEvent(unit, 0x80, 69, 0, 0) == noErr, "note off");
        AudioTimeStamp stamp{};
        stamp.mFlags = kAudioTimeStampSampleTimeValid;
        stamp.mSampleTime = block * 128;
        AudioUnitRenderActionFlags flags = 0;
        check(AudioUnitRender(unit, &flags, &stamp, 0, 128, (AudioBufferList *)&buffers) == noErr,
              "render");
        for (int n = 0; n < 128; ++n)
        {
            check(std::isfinite(left[n]) && left[n] == right[n], "stereo finite");
            if (block == 0 && n < 32)
                check(left[n] == 0, "AU event offset");
            peak = std::max(peak, std::abs(left[n]));
        }
    }
    check(peak > .001 && peak < 1, "audible level");
    check(std::abs(left[127]) < 1e-8, "released silence");
    UInt32 bytes = 0;
    check(AudioUnitGetPropertyInfo(unit, kAudioUnitProperty_ParameterList, kAudioUnitScope_Global,
                                   0, &bytes, nullptr) == noErr &&
              bytes == prism::paramCount * sizeof(AudioUnitParameterID),
          "all host parameters");
    // Exercise the same AU ClassInfo property used by hosts for session persistence.
    std::vector<AudioUnitParameterID> ids(bytes / sizeof(AudioUnitParameterID));
    check(AudioUnitGetProperty(unit, kAudioUnitProperty_ParameterList, kAudioUnitScope_Global, 0,
                               ids.data(), &bytes) == noErr,
          "read parameter IDs");
    bool edited = false;
    for (auto id : ids)
    {
        AudioUnitParameterInfo info{};
        UInt32 infoBytes = sizeof(info);
        check(AudioUnitGetProperty(unit, kAudioUnitProperty_ParameterInfo, kAudioUnitScope_Global,
                                   id, &info, &infoBytes) == noErr,
              "parameter info");
        if (!edited && (info.flags & kAudioUnitParameterFlag_IsWritable) &&
            std::strstr(info.name, "Cutoff"))
        {
            check(AudioUnitSetParameter(unit, id, kAudioUnitScope_Global, 0,
                                        info.minValue + .37f * (info.maxValue - info.minValue),
                                        0) == noErr,
                  "edit cutoff");
            edited = true;
        }
        if (info.cfNameString && (info.flags & kAudioUnitParameterFlag_CFNameRelease))
            CFRelease(info.cfNameString);
    }
    check(edited, "found editable cutoff");
    std::vector<float> expected(ids.size());
    for (size_t i = 0; i < ids.size(); ++i)
        check(AudioUnitGetParameter(unit, ids[i], kAudioUnitScope_Global, 0, &expected[i]) == noErr,
              "capture parameter");
    CFPropertyListRef state = nullptr;
    UInt32 stateBytes = sizeof(state);
    check(AudioUnitGetProperty(unit, kAudioUnitProperty_ClassInfo, kAudioUnitScope_Global, 0,
                               &state, &stateBytes) == noErr &&
              state,
          "get AU session state");
    auto serialized =
        CFPropertyListCreateData(nullptr, state, kCFPropertyListBinaryFormat_v1_0, 0, nullptr);
    check(serialized, "serialize AU session state");
    auto restored = CFPropertyListCreateWithData(nullptr, serialized, kCFPropertyListImmutable,
                                                 nullptr, nullptr);
    check(restored, "deserialize AU session state");
    AudioUnit recalled = nullptr;
    check(AudioComponentInstanceNew(comp, &recalled) == noErr, "fresh recall instance");
    check(AudioUnitSetProperty(recalled, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Output, 0,
                               &fmt, sizeof(fmt)) == noErr,
          "recall format");
    check(AudioUnitSetProperty(recalled, kAudioUnitProperty_ClassInfo, kAudioUnitScope_Global, 0,
                               &restored, sizeof(restored)) == noErr,
          "restore AU state");
    check(AudioUnitInitialize(recalled) == noErr, "recall initialize");
    for (size_t i = 0; i < ids.size(); ++i)
    {
        float actual = 0;
        check(AudioUnitGetParameter(recalled, ids[i], kAudioUnitScope_Global, 0, &actual) == noErr,
              "read recalled parameter");
        check(std::abs(actual - expected[i]) <= 1e-5f * std::max(1.f, std::abs(expected[i])),
              "AU session parameter mismatch");
    }
    check(MusicDeviceMIDIEvent(recalled, 0x90, 60, 100, 0) == noErr, "recalled note on");
    float recalledPeak = 0;
    for (int block = 0; block < 200; ++block)
    {
        AudioTimeStamp stamp{};
        stamp.mFlags = kAudioTimeStampSampleTimeValid;
        stamp.mSampleTime = block * 128;
        AudioUnitRenderActionFlags flags = 0;
        check(AudioUnitRender(recalled, &flags, &stamp, 0, 128, (AudioBufferList *)&buffers) ==
                  noErr,
              "recalled render");
        for (int n = 0; n < 128; ++n)
        {
            check(std::isfinite(left[n]) && std::isfinite(right[n]), "recalled finite");
            recalledPeak = std::max(recalledPeak, std::abs(left[n]));
        }
    }
    check(recalledPeak > .001 && recalledPeak < 1, "recalled output level");
    AudioUnitUninitialize(recalled);
    AudioComponentInstanceDispose(recalled);
    CFRelease(restored);
    CFRelease(serialized);
    CFRelease(state);
    std::printf("AU binary-plist session state restored %zu parameters (including an edited "
                "cutoff) into a fresh "
                "instance and rendered finite audio; peak=%f\n",
                ids.size(), recalledPeak);
    AudioUnitUninitialize(unit);
    AudioComponentInstanceDispose(unit);
    CFRelease(bundle);
    CFRelease(url);
    std::printf("AU direct bundle instantiate, MIDI offset, stereo render, release, all parameters "
                "passed; peak=%f\n",
                peak);
}
