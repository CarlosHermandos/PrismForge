# Validation record — 2026-11-02 (America/Vancouver)

Environment: MacBook Pro Mac17,2, Apple M5 / 10 CPU cores, 16 GB RAM; macOS 27.2; Xcode 27.2 / AppleClang 21.0; CMake Release, native arm64, JUCE 9.0.5. No Windows, Intel, or other-host equivalence claim.

## Builds and native formats
- AU, VST3 and standalone Release targets compile. AU/standalone have local ad-hoc signatures; VST3 uses JUCE's manifest/signing step. `codesign --verify --deep --strict` checks are recorded during packaging. No notarization or distribution certificate.
- **Apple `auval -strict -v aumu PrF1 PrFg`: exit 0, AU VALIDATION SUCCEEDED.** It exercised factory presets, parameters/scheduling/ramping APIs, buses, format changes, render/MIDI behavior and lifecycle. Complete output: `auval.log`. Passing scheduling APIs is not evidence of sample-accurate APVTS automation; processor parameter snapshots remain block accurate.
- The final validator listed all 113 programs and reported component version 0.3.0. It finished with no warnings. An earlier run exposed one-ULP default-retention warnings for skewed parameters; StableParameter now retains the exact normalized host value, and an explicit all-parameter default-retention test passes.
- Direct AU bundle load/instance, 48 kHz stereo rendering, sample-32 note-on, release-to-silence and all 551 parameters passed. Peak .063060. This harness requires unsandboxed AudioUnit instantiation on this machine. See `au-results.log`.
- Direct **VST3** CFBundle/module/factory creation, stereo bus/process setup, sample-32 note-on, finite stereo output, release and component getState/setState APIs passed. Peak .064321. See `vst3-results.log`; this is a small native test host, not the full Steinberg validator or a DAW test.
- The updated AU was installed into the user's Components folder after approval. The old installed component was moved into `Backups/installed-AU`, preserving it. This installation step did not close or modify a Logic project. Subsequent GUI testing is documented below.

## Automated suites
**6/6 CTest suites pass.** `tests.log` contains verbose output and every factory example's peak/RMS results.

1. **Core:** A4 within one cent at 44.1/48/96 kHz; ADSR release, sustain, overlapping same-pitch FIFO, 16-voice stealing, panic, bend, filter stress, all 113 five-second factory examples with finite/level/stuck-note checks.
2. **Processor:** offsets, variable 1/7/64/257/1024/3 blocks at three rates; all 113 program/state round trips; invalid/future state rejection; schema-1 migration; multiple instances; editor create/resize/destroy audio equivalence; queue overflow recovery. Instrumented C++ new/delete observe **zero allocations and deallocations** during wavetable, sample, granular and spectral rendering. This does not instrument every malloc/system/wrapper operation.
3. **Instrument:** independent oscillators, deterministic stereo unison, warp/phase/ring behavior, LP/HP/BP/notch, second filter, 29 source configurations and polarity, every factory sound at three rates, each of eight effects changes output, mono fallback.
4. **Assets:** FFT mip preparation, bounded payload codec and malformed rejection, missing files, embedded-state audio equivalence, and 20 live prepared table replacements.
5. **Hybrid:** sample/multisample/granular/spectral output at three rates; extreme density/size; zones; real spectral analysis and serialization; SFZ supported subset/unsupported-opcode/missing-file reporting; MPE channels/master panic; one-shot early-release behavior and effect-tail panic.
6. **Sequencer:** exact note-on and gate samples, up order, panic flush, alternating swing boundaries, reproducible probability, latch and transport stop.

AddressSanitizer + UndefinedBehaviorSanitizer build of the instrument suite (`clang++ -O1 -g -fsanitize=address,undefined`) passes with no diagnostics; `sanitizer.log`. This is targeted memory/UB coverage, not exhaustive race/leak certification.

All ten pages were exercised by their real button callbacks and rendered to PNG. Direct-child bounds were checked at 1000×630, 1180×740 and 1440×900; snapshots are `editor*.png`. Main, preset and table pages were visually inspected. File dialogs, full drag/drop gestures, physical high-DPI screens and screen readers were not manually audited.

## Measurements and benchmarks
`benchmark.csv` contains six documented patch configurations at 44.1/48/96 kHz, 128-sample blocks, half-second warmup and 100 timed blocks each. Core only; host, UI and device overhead excluded. These are observations on this machine, not worst-case real-time guarantees.

At 48 kHz / 16 voices: one oscillator mean 441.353 µs / observed peak 452.458 µs; three oscillators × seven unison mean 889.508 µs / peak 905.583 µs against a 2666.67 µs block deadline. At 96 kHz, the same maximum-unison configuration peaked at **912.458 µs** against **1333.33 µs**. The earlier implementation peaked at 1451.88 µs and missed that deadline; shared detune/mip calculations, equal-power panning and recursive grain windows resolved the observed case. Before/after raw files are preserved. Scheduling noise and other workloads can still cause larger peaks.

The coherent table sweep uses 8192 samples, four exact FFT-bin frequencies and four table positions at each sample rate. It records energy outside the retained harmonic bins. See the CSV's second section. It characterizes unwarped tables/linear interpolation only; no nonlinear warp/FM/drive/saturation alias-free claim follows. Hard sync and nonlinear processing need additional quality measurements and anti-aliasing refinement.

`renders/0.wav` through `112.wav` are five-second 48 kHz mono examples generated from the actual engine. Bass examples use C2/MIDI36, other examples MIDI60; arp patches internally schedule their notes. These and factory presets have been measured, **not auditioned**. Native stereo rendering is tested separately.

## Remaining validation
The earlier computer-control permission blocker is resolved. A basic in-Logic AU GUI smoke test passed: editor load, Init → Foundation preset/parameter change, FX view, and editor close/reopen. MIDI-region playback with channel/output levels was observed in a separate Untitled test project. See `logic-smoke.md` for scope and limitations. A follow-up Logic save/close/reopen test restored Foundation with an edited 2345 Hz cutoff; the project and screenshot are retained beside the smoke report. Imported-asset session recall, standalone audio-device/listening testing and manual auditory-quality sign-off remain open. External DAW automation/editing/multi-instance/seek/loop combinations, full pluginval/Steinberg validation, hostile-input long soaks, race detection and accessibility checks remain open. No user Library cache was deleted or unrelated host process killed.

## 2026-11-03 build verification
CTest now includes AUIntegration and VST3Integration (eight tests total). AUIntegration serializes the actual AU ClassInfo property as a binary plist, restores into a fresh instance, checks all 551 parameters including an edited cutoff, and renders finite non-silent audio. This is separate from the Logic project recall test. The rebuilt interface includes an embedded AGPLv3 license/notices viewer. The distribution includes the JUCE 9.0.5 dependency source and CMake selects it when rebuilding the extracted Source Project.

Final UI build: seven checks passed in the sandbox; AUIntegration was blocked by macOS AU instantiation there (status -3000) and passed when rerun outside the sandbox (`native-retry.log`). All eight checks therefore have passing results on the final build. Strict auval also passed after installing that build.

Standalone GUI check on 2026-11-03: launched the final app through native computer control, opened the LICENSE view, observed project copyright, AGPL-3.0-only and warranty notices in accessible text, then closed the view and returned to the instrument. This verifies the embedded license viewer; it is not an audio-device/listening sign-off.

The packaged Source Project configured successfully in a fresh external build directory. Its CMake cache selected the included ThirdParty/JUCE, and juceaide was built successfully. This checks the dependency source/build setup; the complete packaged-source target rebuild was not repeated.

## EDM expansion verification — 2026-11-03
All 113 patches passed five-second finite/level/release renders and three-rate rendering. The 48 new recipes additionally passed four-note stereo playback with ordinary note-offs and parameter-range checks. Processor recall initially exposed the 0.001f/decimal-XML boundary issue; validation now compares bounds at parameter float precision, and a regression check round-trips every parameter minimum and maximum. All 113 program round trips and the processor/asset/hybrid/AU/VST3 follow-up checks passed (`edm-recall-results.log`). The earlier failure remains recorded in tests.log. `edm-render-metrics.csv` measures the 48 PCM16 preview files; no manual audition is claimed. Strict auval passed after the updated AU installation.

## Category UI and distribution cleanup — 2026-11-03
Processor tests verify nine category submenus contain all 113 unique factory program IDs, Brass filtering loads a Brass patch, and empty search/reset works. All factory state round trips and existing processor checks pass. Snapshots show the full PATCH browser and the smaller details layout. Native AU/VST3 integration and strict auval pass after installation. Sound recipes and host program numbers are unchanged.

The end-user ZIP contains AU, VST3, Standalone, Install AU.command, START HERE.txt, Licenses, a nested corresponding-source ZIP, and SHA256SUMS.txt. Duplicate factory exports, audio examples, development logs/screenshots, Logic test projects and expanded developer files are omitted from the main download. Technical documentation, tests, project source and JUCE sources remain in the nested source archive. Existing extracted user copies in dist were not deleted.

## Preset selection flow — 0.3.1
The top preset selector is now a Browse button that opens the category browser directly, replacing the nested popup. Category buttons filter sounds; clicking a named row loads it. Accessible rows expose preset names instead of generic row numbers. The result label explicitly explains that a sound must be clicked to load. The main preset name follows the loaded processor state. Earlier category-menu tests only checked menu inventory; the updated regression test enters through the real editor's Browse button and checks that selecting EDM Arena Brass changes both preset metadata and its 900 Hz cutoff. No claim is made that the previous popup failure's exact cause was reproduced.

### 0.3.1 live verification
The final standalone app loaded EDM Clean Sub through Browse → Bass → named row, changing the displayed name, table to 0 and cutoff to 400 Hz. After the user closed Logic, the 0.3.1 AU was installed with a backup and passed strict auval. Logic reopened the isolated PrismForge Recall project, preserving Foundation and its edited 2345 Hz cutoff. Browse → Brass → EDM Chrome Horns changed the displayed name and controls to cutoff 1800 Hz, oscillator 1 unison 3 and oscillator 2 pitch +12. `logic-presets-031.png` records the loaded patch. The user's own project was not modified.
