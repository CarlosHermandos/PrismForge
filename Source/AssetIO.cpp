#include "AssetIO.h"
namespace prism
{
juce::String encodeAsset(const Asset &asset)
{
    juce::MemoryOutputStream raw;
    raw.writeInt(0x50464153);
    raw.writeInt(1);
    raw.writeInt(asset.kind);
    raw.writeInt(asset.root);
    raw.writeInt(int(asset.frames.size()));
    for (auto &frame : asset.frames)
    {
        raw.writeInt(int(frame.size()));
        for (float v : frame)
            raw.writeFloat(v);
    }
    raw.writeInt(int(asset.zones.size()));
    for (auto &zone : asset.zones)
    {
        raw.writeDouble(zone.rate);
        for (int v : {zone.root, zone.low, zone.high, zone.lowVelocity, zone.highVelocity,
                      zone.loopStart, zone.loopEnd})
            raw.writeInt(v);
        raw.writeFloat(zone.tune);
        raw.writeFloat(zone.gain);
        raw.writeBool(zone.loop);
        raw.writeInt(int(zone.l.size()));
        for (size_t i = 0; i < zone.l.size(); ++i)
        {
            raw.writeFloat(zone.l[i]);
            raw.writeFloat(zone.r[i]);
        }
    }
    raw.writeInt(int(asset.spectrum.size()));
    raw.writeFloat(asset.spectrumHop);
    for (auto &frame : asset.spectrum)
        for (float v : frame)
            raw.writeFloat(v);
    juce::MemoryOutputStream packed;
    {
        juce::GZIPCompressorOutputStream compressor(packed, 6);
        compressor.write(raw.getData(), raw.getDataSize());
    }
    return packed.getMemoryBlock().toBase64Encoding();
}
std::unique_ptr<Asset> decodeAsset(const juce::String &text, juce::String &error)
{
    juce::MemoryBlock packed;
    if (!packed.fromBase64Encoding(text) || packed.getSize() > 64 * 1024 * 1024)
    {
        error = "Invalid or oversized asset";
        return {};
    }
    juce::MemoryInputStream source(packed, false);
    juce::GZIPDecompressorInputStream unzip(source);
    juce::MemoryOutputStream decoded;
    std::array<char, 32768> buffer;
    while (!unzip.isExhausted())
    {
        int n = unzip.read(buffer.data(), int(buffer.size()));
        if (n <= 0)
            break;
        if (decoded.getDataSize() + size_t(n) > 64 * 1024 * 1024)
        {
            error = "Asset exceeds decoded size limit";
            return {};
        }
        decoded.write(buffer.data(), size_t(n));
    }
    juce::MemoryInputStream in(decoded.getData(), decoded.getDataSize(), false);
    auto fail = [&]()
    {
        error = "Malformed asset payload";
        return std::unique_ptr<Asset>{};
    };
    if (in.readInt() != 0x50464153 || in.readInt() != 1)
        return fail();
    auto a = std::make_unique<Asset>();
    int kind = in.readInt();
    if (kind < 0 || kind > 3)
        return fail();
    a->kind = Asset::Kind(kind);
    a->root = in.readInt();
    int count = in.readInt();
    if (count < 0 || count > 64 || a->root < 0 || a->root > 127)
        return fail();
    for (int f = 0; f < count; ++f)
    {
        int n = in.readInt();
        if (n < 32 || n > 8192 || in.getNumBytesRemaining() < n * 4)
            return fail();
        std::vector<float> frame(size_t(n), 0);
        for (auto &v : frame)
        {
            v = in.readFloat();
            if (!std::isfinite(v) || std::abs(v) > 4)
                return fail();
        }
        a->frames.push_back(std::move(frame));
    }
    int zones = in.readInt();
    if (zones < 0 || zones > 128)
        return fail();
    size_t total = 0;
    for (int z = 0; z < zones; ++z)
    {
        Asset::Zone zone;
        zone.rate = in.readDouble();
        zone.root = in.readInt();
        zone.low = in.readInt();
        zone.high = in.readInt();
        zone.lowVelocity = in.readInt();
        zone.highVelocity = in.readInt();
        zone.loopStart = in.readInt();
        zone.loopEnd = in.readInt();
        zone.tune = in.readFloat();
        zone.gain = in.readFloat();
        zone.loop = in.readBool();
        int n = in.readInt();
        if (n < 1 || n > 4000000 || in.getNumBytesRemaining() < int64_t(n) * 8 ||
            !std::isfinite(zone.rate) || zone.rate < 8000 || zone.rate > 192000 || zone.root < 0 ||
            zone.root > 127 || zone.low < 0 || zone.high > 127 || zone.low > zone.high ||
            zone.lowVelocity < 0 || zone.highVelocity > 127 ||
            zone.lowVelocity > zone.highVelocity || !std::isfinite(zone.tune) ||
            std::abs(zone.tune) > 1200 || !std::isfinite(zone.gain) || zone.gain < 0 ||
            zone.gain > 8)
            return fail();
        total += size_t(n);
        if (total > 6000000)
            return fail();
        zone.l.resize(size_t(n));
        zone.r.resize(size_t(n));
        for (int i = 0; i < n; ++i)
        {
            zone.l[size_t(i)] = in.readFloat();
            zone.r[size_t(i)] = in.readFloat();
            if (!std::isfinite(zone.l[size_t(i)]) || !std::isfinite(zone.r[size_t(i)]) ||
                std::abs(zone.l[size_t(i)]) > 4 || std::abs(zone.r[size_t(i)]) > 4)
                return fail();
        }
        if (zone.loop &&
            (zone.loopStart < 0 || zone.loopEnd > n || zone.loopEnd - zone.loopStart < 4))
            return fail();
        a->zones.push_back(std::move(zone));
    }
    int spectral = in.readInt();
    a->spectrumHop = in.readFloat();
    if (spectral < 0 || spectral > 100000 || !std::isfinite(a->spectrumHop) ||
        a->spectrumHop < .001f || a->spectrumHop > 1 ||
        in.getNumBytesRemaining() < int64_t(spectral) * 32 * 4)
        return fail();
    a->spectrum.resize(size_t(spectral));
    for (auto &frame : a->spectrum)
        for (auto &v : frame)
        {
            v = in.readFloat();
            if (!std::isfinite(v) || v < 0 || v > 8)
                return fail();
        }
    if (in.getNumBytesRemaining() != 0)
        return fail();
    if (a->kind == Asset::wavetable && !a->prepareTables(false))
        return fail();
    if (a->kind != Asset::wavetable && a->zones.empty())
        return fail();
    if (a->kind == Asset::spectral && a->spectrum.empty())
        return fail();
    if (!a->zones.empty() && !a->prepareSamples())
        return fail();
    return a;
}
std::unique_ptr<Asset> importWavetable(const juce::File &file, int frameSize, bool normalize,
                                       juce::String &error)
{
    if (frameSize < 32 || frameSize > 8192 || (frameSize & (frameSize - 1)))
    {
        error = "Frame size must be a power of two from 32 to 8192";
        return {};
    }
    juce::AudioFormatManager formats;
    formats.registerBasicFormats();
    std::unique_ptr<juce::AudioFormatReader> reader(formats.createReaderFor(file));
    if (!reader)
    {
        error = "Cannot decode WAV/AIFF file: " + file.getFileName();
        return {};
    }
    if (reader->lengthInSamples < frameSize || reader->lengthInSamples % frameSize ||
        reader->lengthInSamples / frameSize > 64)
    {
        error = "Audio must contain 1-64 complete frames of the selected frame size";
        return {};
    }
    juce::AudioBuffer<float> buffer(int(std::min(2u, reader->numChannels)),
                                    int(reader->lengthInSamples));
    if (!reader->read(&buffer, 0, buffer.getNumSamples(), 0, true, true))
    {
        error = "Audio decoding failed";
        return {};
    }
    auto a = std::make_unique<Asset>();
    for (int f = 0; f < buffer.getNumSamples() / frameSize; ++f)
    {
        std::vector<float> frame(size_t(frameSize), 0);
        for (int i = 0; i < frameSize; ++i)
        {
            float value = 0;
            for (int c = 0; c < buffer.getNumChannels(); ++c)
                value += buffer.getSample(c, f * frameSize + i) / buffer.getNumChannels();
            if (!std::isfinite(value))
            {
                error = "Wavetable contains a nonfinite sample";
                return {};
            }
            frame[size_t(i)] = value;
        }
        a->frames.push_back(std::move(frame));
    }
    if (!a->prepareTables(normalize))
    {
        error = "Could not prepare wavetable";
        return {};
    }
    return a;
}
} // namespace prism

#include <map>
#include <regex>
#include <set>
namespace prism
{
std::unique_ptr<Asset> importSample(const juce::File &file, bool spectralMode, juce::String &error,
                                    int root)
{
    juce::AudioFormatManager formats;
    formats.registerBasicFormats();
    std::unique_ptr<juce::AudioFormatReader> reader(formats.createReaderFor(file));
    if (!reader)
    {
        error = "Cannot decode sample: " + file.getFullPathName();
        return {};
    }
    if (reader->sampleRate < 8000 || reader->sampleRate > 192000 || reader->lengthInSamples < 32 ||
        reader->lengthInSamples > reader->sampleRate * 8 || reader->lengthInSamples > 4000000)
    {
        error = "Sample must be 32 samples to 8 seconds, at 8-192 kHz";
        return {};
    }
    juce::AudioBuffer<float> buffer(2, int(reader->lengthInSamples));
    if (!reader->read(&buffer, 0, buffer.getNumSamples(), 0, true, true))
    {
        error = "Sample decoding failed";
        return {};
    }
    auto asset = std::make_unique<Asset>();
    asset->kind = spectralMode ? Asset::spectral : Asset::sample;
    asset->root = std::clamp(root, 0, 127);
    Asset::Zone zone;
    zone.root = asset->root;
    zone.rate = reader->sampleRate;
    zone.l.resize(size_t(buffer.getNumSamples()));
    zone.r.resize(zone.l.size());
    zone.loopEnd = buffer.getNumSamples();
    for (int n = 0; n < buffer.getNumSamples(); ++n)
    {
        zone.l[size_t(n)] = buffer.getSample(0, n);
        zone.r[size_t(n)] = reader->numChannels > 1 ? buffer.getSample(1, n) : zone.l[size_t(n)];
        if (!std::isfinite(zone.l[size_t(n)]) || !std::isfinite(zone.r[size_t(n)]) ||
            std::abs(zone.l[size_t(n)]) > 4 || std::abs(zone.r[size_t(n)]) > 4)
        {
            error = "Sample has nonfinite values or exceeds +/-4; normalize it before import";
            return {};
        }
    }
    if (spectralMode)
    {
        constexpr int size = 2048, hop = 512;
        asset->spectrumHop = float(hop / zone.rate);
        std::vector<std::complex<float>> bins(size);
        double fundamental = 440 * std::exp2((asset->root - 69) / 12.);
        for (size_t start = 0; start < zone.l.size(); start += hop)
        {
            double windowSum = 0;
            for (int i = 0; i < size; ++i)
            {
                float window = float(.5 - .5 * std::cos(2 * 3.141592653589793 * i / (size - 1)));
                size_t index = start + size_t(i);
                bins[size_t(i)] =
                    (index < zone.l.size() ? (zone.l[index] + zone.r[index]) * .5f : 0) * window;
                windowSum += window;
            }
            Asset::fft(bins, false);
            std::array<float, 32> frame{};
            for (int h = 1; h <= 32; ++h)
            {
                double bin = fundamental * h / zone.rate * size;
                if (bin >= size / 2)
                    break;
                int centre = int(std::round(bin));
                double power = 0;
                for (int k = std::max(1, centre - 1); k <= std::min(size / 2 - 1, centre + 1); ++k)
                    power += std::norm(bins[size_t(k)]);
                frame[size_t(h - 1)] = float(std::sqrt(power) * 2 / windowSum / 1.224744871);
            }
            asset->spectrum.push_back(frame);
        }
    }
    asset->zones.push_back(std::move(zone));
    if (!asset->prepareSamples())
    {
        error = "Sample mip construction failed";
        return {};
    }
    return asset;
}
std::unique_ptr<Asset> importSFZ(const juce::File &file, juce::String &error)
{
    if (!file.existsAsFile() || file.getSize() > 1024 * 1024)
    {
        error = "Missing or oversized SFZ";
        return {};
    }
    auto content = file.loadFileAsString().toStdString();
    content = std::regex_replace(content, std::regex("//[^\\r\\n]*"), "");
    std::regex token(R"sfz(<(global|group|region)>|([A-Za-z0-9_]+)\s*=\s*("[^"]*"|[^\s<]+))sfz");
    std::map<std::string, std::string> global, group, region;
    auto *current = &global;
    bool inRegion = false;
    auto asset = std::make_unique<Asset>();
    asset->kind = Asset::multisample;
    std::set<std::string> unsupported;
    size_t total = 0;
    auto note = [](std::string value)
    {
        if (std::regex_match(value, std::regex("[0-9]+")))
            return std::stoi(value);
        std::smatch match;
        if (std::regex_match(value, match, std::regex("([A-Ga-g])([#b]?)(-?[0-9]+)")))
        {
            auto letter = std::toupper(match[1].str()[0]);
            int pitch = letter == 'C'   ? 0
                        : letter == 'D' ? 2
                        : letter == 'E' ? 4
                        : letter == 'F' ? 5
                        : letter == 'G' ? 7
                        : letter == 'A' ? 9
                                        : 11;
            if (match[2] == "#")
                ++pitch;
            if (match[2] == "b")
                --pitch;
            return (std::stoi(match[3]) + 1) * 12 + pitch;
        }
        return -1;
    };
    auto commit = [&]()
    {
        if (!inRegion)
            return true;
        if (asset->zones.size() >= 128)
        {
            error = "SFZ exceeds 128 regions";
            return false;
        }
        if (!region.count("sample"))
        {
            error = "SFZ region has no sample";
            return false;
        }
        auto path = juce::String(region["sample"]).replaceCharacter('\\', '/');
        auto source = file.getParentDirectory().getChildFile(path);
        juce::String message;
        auto decoded = importSample(source, false, message);
        if (!decoded)
        {
            error = "SFZ: " + message;
            return false;
        }
        auto zone = std::move(decoded->zones[0]);
        auto integer = [&](const char *key, int fallback)
        { return region.count(key) ? note(region[key]) : fallback; };
        zone.root = integer("pitch_keycenter", 60);
        zone.low = integer("lokey", 0);
        zone.high = integer("hikey", 127);
        if (region.count("key"))
            zone.low = zone.high = zone.root = note(region["key"]);
        zone.lowVelocity = integer("lovel", 0);
        zone.highVelocity = integer("hivel", 127);
        auto number = [&](const char *key, float fallback)
        {
            if (!region.count(key))
                return fallback;
            size_t position = 0;
            float value = std::stof(region[key], &position);
            if (position != region[key].size() || !std::isfinite(value))
                throw std::invalid_argument("number");
            return value;
        };
        zone.tune = number("tune", 0);
        zone.gain = std::pow(10.f, number("volume", 0) / 20);
        if (region.count("loop_mode"))
        {
            auto mode = region["loop_mode"];
            if (mode != "no_loop" && mode != "one_shot" && mode != "loop_continuous")
            {
                error = "Unsupported SFZ loop_mode: " + juce::String(mode);
                return false;
            }
            zone.loop = mode == "loop_continuous";
        }
        zone.loopStart =
            region.count("loop_start") ? juce::String(region["loop_start"]).getIntValue() : 0;
        zone.loopEnd = region.count("loop_end") ? juce::String(region["loop_end"]).getIntValue() + 1
                                                : int(zone.l.size());
        if (zone.root < 0 || zone.root > 127 || zone.low < 0 || zone.high > 127 ||
            zone.low > zone.high || zone.lowVelocity < 0 || zone.highVelocity > 127 ||
            zone.lowVelocity > zone.highVelocity || std::abs(zone.tune) > 1200 ||
            !std::isfinite(zone.gain) || zone.gain > 8 ||
            (zone.loop && (zone.loopStart < 0 || zone.loopEnd > int(zone.l.size()) ||
                           zone.loopEnd - zone.loopStart < 4)))
        {
            error = "Invalid SFZ zone bounds/tuning/loop";
            return false;
        }
        total += zone.l.size();
        if (total > 6000000)
        {
            error = "SFZ exceeds six million decoded stereo frames";
            return false;
        }
        asset->zones.push_back(std::move(zone));
        return true;
    };
    try
    {
        for (auto it = std::sregex_iterator(content.begin(), content.end(), token);
             it != std::sregex_iterator(); ++it)
        {
            auto m = *it;
            if (m[1].matched)
            {
                if (!commit())
                    return {};
                auto tag = m[1].str();
                inRegion = tag == "region";
                if (tag == "global")
                {
                    global.clear();
                    current = &global;
                }
                else if (tag == "group")
                {
                    group = global;
                    current = &group;
                }
                else
                {
                    region = group.empty() ? global : group;
                    current = &region;
                }
            }
            else
            {
                auto key = m[2].str(), value = m[3].str();
                if (!value.empty() && value.front() == '"')
                    value = value.substr(1, value.size() - 2);
                static const std::set<std::string> supported{
                    "sample",          "key",  "lokey",  "hikey",     "lovel",      "hivel",
                    "pitch_keycenter", "tune", "volume", "loop_mode", "loop_start", "loop_end"};
                if (!supported.count(key))
                    unsupported.insert(key);
                else
                    (*current)[key] = value;
            }
        }
        if (!commit())
            return {};
    }
    catch (const std::exception &)
    {
        error = "Malformed SFZ numeric value";
        return {};
    }
    if (asset->zones.empty())
    {
        error = "SFZ contains no regions";
        return {};
    }
    if (!unsupported.empty())
    {
        error = "Imported; unsupported opcodes ignored: ";
        for (auto &opcode : unsupported)
            error += juce::String(opcode) + " ";
    }
    return asset;
}
} // namespace prism
