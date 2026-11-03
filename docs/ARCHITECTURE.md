# Prism Forge 0.3 architecture

C++20, JUCE 9.0.5 and CMake. An original native hybrid instrument; PulseDuck remains untouched. The initial implementation is preserved in `Backups/foundation`; the core completion plan is recorded in `docs/NEXT-MILESTONE.md`.

## Layers
- `Plugin`: AU/VST3/standalone integration, 551 stable parameter IDs, atomic block snapshots, sample-offset MIDI, a bounded 256-event UI SPSC queue, tempo/transport, meters and session serialization. All editor code is independent of rendering.
- `StableParameter`: exact normalized host-value retention with JUCE native-range/APVTS integration; avoids skew round-trip default warnings.
- `Parameters`: central ranges, units, defaults, skew, discrete step, choice labels, explicit modulation destination registry, and 64 original presets plus Init. Generated full references are in `Content/Parameters.tsv` and `Content/Presets.tsv`.
- `Engine`: fixed 16-voice storage, poly/mono/legato/retrigger/glide, channel MIDI/MPE expression, three oscillator slots, sub/noise, four envelopes, ten LFOs, sixteen modulation routes and dual per-voice stereo filters.
- `Hybrid` / `Assets`: mathematical or imported mipmapped tables, mipmapped cubic sample playback, key/velocity zones, 64-grain pools per voice/oscillator, and 32-harmonic additive spectral synthesis.
- `Effects`: global reorderable serial saturation, chorus, phaser, stereo delay, four-line feedback-network reverb, low/high shelves plus parametric middle EQ, compressor and optional peak ceiling.
- `AssetIO`: validated decoding, FFT mip construction, harmonic analysis, bounded SFZ subset and compressed portable asset serialization on control/background threads.
- Native editor, preset browser, wavetable editor and output scope/spectrum are separate components. The UI only reads atomic levels, bounded display samples and pinned immutable assets.
- Offline test, content-export and benchmark executables are separate CMake targets.

## Signal and modulation order
Incoming MIDI/keyboard -> optional internal arp/pattern generator -> voice allocation -> four envelopes and ten LFOs -> normalized sixteen-route matrix -> oscillator A -> B -> C -> sub/noise -> per-source amplitude -> filter routing -> stereo dual filters -> voice summation -> master headroom -> serial effects -> host output. Phase/ring modulation only consumes the preceding oscillator; no cycles or feedback modulation paths exist. Envelope 1 controls the amplitude; envelopes 2-4 are routable sources.

Timbre, tuning, levels, resonance, cutoff and all other continuous controls receive 5 ms exponential smoothing. Per-voice matrix evaluation and oscillator interaction are sample-rate calculations. MIDI events are sample accurate. APVTS host automation is block accurate. LFO rate sync uses host BPM or standalone internal tempo; missing host beat positions fall back to an internal running clock, with approximate reanchoring from sample position after seeks.

## Real-time ownership
The callback uses fixed voices, grains, routes, keyboard events and scope storage. Delay/reverb buffers are allocated in prepareToPlay. Project rendering code performs no file I/O, logging, locks, heap allocation or asset destruction. The tests instrument C++ new/delete during wavetable, sample, granular and spectral rendering; this is not a claim that every host-wrapper/system allocation is covered.

Each oscillator has three asset slots with atomic free/writing/ready/active/retired states. Only the control/worker thread owns and replaces unique_ptr payloads. Ready commands carry a monotonically changing token and parameter-state version. The processor accepts prepared assets at a block boundary alongside the corresponding complete parameter snapshot. Audio only reads immutable data and changes bounded slot states. Old tables crossfade; hybrid asset changes use a 128-sample transition. UI/export pins prevent slot reuse during reads. The audio thread never deletes an asset.

Preset parsing and asset reconstruction occur outside processBlock. A state sequence counter keeps partially updated control settings out of audio snapshots. Background asset jobs carry per-oscillator request generations so an old file-load result cannot overwrite a newer preset. User preset and wavetable writes occur only through explicit UI file operations.

Panic invalidates delay-buffer generations in constant time, instead of clearing large buffers in the callback. Nonfinite input parameters are sanitized; DSP nonfinite output resets affected state. No master limiter is enabled by default.

## Intentional boundaries
This is a local engineering build, not commercial-synth equivalence or release certification. MPE is manually configured lower-zone channel expression, SFZ is an explicit subset, spectral resynthesis is harmonic and lossy, sample import is bounded, and nonlinear anti-aliasing remains an area for further quality work. See STATUS and VALIDATION for exact evidence and gaps.
