# Prism Forge 0.3.1 status

| Area | Implemented | Evidence / status |
|---|---|---|
| Native builds | AU, VST3, standalone, configurable identity | Built arm64 Release; local ad-hoc signatures; no Windows/x86 claim |
| Oscillators | Three interchangeable engines; mathematical/imported mipmapped tables; tuning/fine/phase/random; 1-7 unison, spread/pan; two serial pulse/phase/sync warps; acyclic phase/ring interaction | Automated output/stereo/seed/interaction tests and coherent table sweep; nonlinear alias quality remains partial |
| Voice controls | 1-16 voices, released/oldest stealing and residuals, mono/legato/retrigger/glide, velocity/release velocity/controllers/sustain/panic | Offset/rate/overlap/sustain/steal/mono tests; long hostile-MIDI soak remains open |
| Dual filters | Independent stereo LP/HP/BP/notch, cutoff/resonance/drive/tracking; source routing and serial/parallel paths | Stress/output tests; ladder, comb and additional character types remain pending |
| Modulation | Four curved envelopes; ten LFOs including editable eight-point shapes, tempo sync, polarity, phase, retrigger and one-shot; eight macros; sixteen routes with polarity/curve/bypass; drag assignment and indicators | Source/depth/polarity, seed and rendering tests; per-route UI interaction/accessibility audit remains open |
| Effects | Saturation, chorus, phaser, delay, algorithmic reverb, parametric middle EQ plus shelves, compressor, optional peak ceiling; serial order persistence and bypass transitions | Every effect changes audio; benchmark suite. Nonlinear oversampling/alias refinement and send/parallel effect buses remain pending |
| Wavetable editor | Frame selection/insert/duplicate/delete/reorder/interpolate, drawing, sixteen harmonic sliders, DC remove/invert/reverse, normalize, undo/redo, WAV import/export, prepared live replacement | FFT/codec and repeated replacement tests; render snapshots. Full gesture-by-gesture UI QA remains open |
| Preset browser | Search/category/tags/favourites, previous/next, user Save As/rename/duplicate/import/export, metadata and macro names, protected factory bank | 113 state round trips and generated real factory exports; file-dialog/favourites persistence manual QA remains open |
| Hybrid engines | Sample root/tuning/range/reverse/one-shot/loop/crossfade; SFZ key/velocity zones; bounded granular engine; genuine harmonic analysis/resynthesis; embedded portable assets | Three-rate tests, zone rejection, real harmonic analysis, SFZ unsupported/missing-file tests and embedded audio equivalence |
| Performance | Arp orders/octaves/gate/swing/latch; sixteen-step relative-note sequencer with velocity/gate/probability; host/internal tempo; stop/seek flush | Exact sample positions/gates/swing, seed/latch/stop tests; external-host seek/loop automation combinations remain open |
| MPE | Manual lower-zone member pitch, pressure and timbre with master expression | Channel expression/master panic tests; automatic RPN negotiation/upper zone not implemented |
| UI | All ten views work; responsive native layout, editable values, fine/reset/tooltips/names; keyboard, voice activity, L/R meters, waveform/FFT | All pages rendered; bounds checked at three sizes. Basic Logic AU editor smoke passed; broader host/high-DPI/screen-reader testing remains open |
| Audio-thread behavior | Fixed voices/grains/routes/events, preallocated effect buffers, versioned pinned asset slots, no audio-thread asset ownership destruction | Instrumented C++ allocation/deallocation checks for four engine types and source audit; wrapper/system allocator instrumentation is incomplete |
| Factory content | Init + 112 original presets: 21 Bass, 8 Brass, 19 Leads, 21 Pads, 17 SFX, 8 Chords, 10 Keys & Plucks, 8 Arps & Sequences; four useful macros each | Single-note five-second renders and clipping/stuck-note checks; all presets render at three rates; no audition claimed |

## Milestone interpretation
The five milestone feature groups now have substantive working implementations. They are not all release-certified. The remaining work is chiefly DSP quality refinement, the explicitly deferred filter/effect types and routing extensions, external host/device/listening tests, accessibility, long stress testing and distribution signing/notarization. No equivalence to a commercial instrument is claimed.

## Known boundaries / reproduction
- Set maximum unison/polyphony, high drive/resonance and a small device buffer: CPU cost and intended distortion increase. Measure in the target DAW; the core benchmark excludes host/UI overhead.
- Enable hard sync, strong phase modulation or saturation at high notes: nonlinear aliasing can be audible. The table-only sweep does not validate these operations as alias-free.
- Load a spectral source containing percussion/noise: the harmonic model loses those components and softens transients. Set the analysis root before importing.
- Select an engine without importing a source: it is silent and the OSC preview says to load a source. Imported/exported saved presets embed audio and do not require the original file.
- SFZ unsupported opcodes are listed, not emulated. Repeated/overlapping regions currently use the first matching region, without layered zone summation or round robin.
- Changing a preset is a control-thread operation. Parameters/assets are accepted together at an audio block boundary; active voices transition rather than receiving an instantaneous hard reset. Loading large embedded states may pause the control/UI thread.
- Editing the effect order into duplicate values intentionally omits the duplicate effect until the order contains each desired effect once.
- MPE uses a manually configured lower zone. MIDI output, upper zone and RPN negotiation are absent.
- Full MIDI 2.0, arbitrary feedback/sends, ladder/comb filters, unrestricted sample/SFZ libraries and full spectral reconstruction are outside this build.
- Editor preferences are not persisted. Core controls have names/focus/tooltips, but a screen-reader audit has not been completed.
- The computer-control permission blocker is resolved. A basic Logic AU smoke test passed: editor loaded, Init → Foundation changed parameters, FX view opened, and editor closed/reopened. MIDI-region playback and channel/output levels were observed. Logic project save/reopen retained Foundation and an edited 2345 Hz cutoff. Automation and listening sign-off remain open; see `logic-smoke.md`.

## Open-source delivery — 2026-11-03
AGPL-3.0-only selected by the owner; full license and notices are included, with JUCE dependency source for rebuilding. The interface now uses an aubergine/lavender/amber palette, a prism mark and hexagonal knob details. AU ClassInfo binary-plist recall verifies all 551 parameters into a fresh instance and renders audio. Native AU/VST3 integration tests are now included in CTest. This is a functional open-source beta; the known DSP/QA boundaries above still apply.

## EDM expansion — 2026-11-03
48 original recipes appended at indices 65–112, eight each in Brass, Bass, Leads, Pads, SFX and Chords. Existing program indices and sound recipes remain intact; Lead and Pad display categories are unified as Leads and Pads. Chord presets use oscillator intervals from a single played root; seventh shells omit the fifth. See EDM-BANK.md.

## Browsing cleanup — 2026-11-03
All factory sounds now use nine consistent categories (including Init). The top menu groups by category without reassigning host program numbers. PATCH uses a category sidebar and alphabetical results, with editing fields collapsed by default. The end-user download has one source archive rather than an expanded developer tree. Tests and evidence remain in the workspace.

## Preset selection flow — 0.3.1
The top preset selector is now a Browse button that opens the category browser directly, replacing the nested popup. Category buttons filter sounds; clicking a named row loads it. Accessible rows expose preset names instead of generic row numbers. The result label explicitly explains that a sound must be clicked to load. The main preset name follows the loaded processor state. Earlier category-menu tests only checked menu inventory; the updated regression test enters through the real editor's Browse button and checks that selecting EDM Arena Brass changes both preset metadata and its 900 Hz cutoff. No claim is made that the previous popup failure's exact cause was reproduced.
