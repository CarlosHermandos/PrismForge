# Native panel interface

The supplied reference informed density and hierarchy: dense oscillator and modulation controls, preset toolbar and a bottom keyboard. Prism Forge uses original native drawing and its own identity. No reference screenshot pixels or third-party artwork are embedded.

All ten views now expose actual implemented functionality: OSC, MIX, FX, MOD, SETUP, PATCH, TABLE, ASSET, ARP and SCOPE. Wavetable-only controls are disabled for hybrid modes; unrelated grain/spectral controls are disabled for other engines. Oscillator A's preceding-oscillator interaction is disabled to prevent a feedback loop. Custom LFO points become editable when Custom shape is selected.

Top source buttons can be dragged onto supported targets; clicking them selects the envelope or jumps to the LFO editor. Macro labels drag their sources. Lavender control marks indicate assigned routes and aggregate configured depth. A drop uses the first unused route at +.25 depth; a full sixteen-route bank leaves existing routes unchanged.

Wave/source previews are parameter/asset views. The mathematical preview shows 24 Fourier harmonics; it is not a scope of a sounding voice's current mip. Filter response is the linear TPT response before drive/modulation. Envelope plots reflect stage curves with an illustrative .35-second hold. SCOPE is the real bounded output waveform plus display-only 512-point FFT. Meter values come from separate atomic left/right peaks.

Default editor size is 1180×740, allowed 1000×630 to 1770×1110. Native components and curves resize without dependence on the audio engine. Editor preferences are not persisted. Offscreen snapshots cover all ten pages and three sizes; physical display, accessibility and full gesture QA remain open.

## 2026-11-03 visual identity
The current design uses aubergine panels, lavender modulation, amber oscillator displays, flat hexagonal knob detailing, rounded section headers and a drawn prism mark. The license button opens the full AGPLv3 text and project notices. These changes distinguish the presentation; they are not an IP clearance opinion.

## Category browser and clean download
All 113 factory programs are grouped into Bass, Brass, Leads, Pads, Keys & Plucks, Chords, Arps & Sequences, SFX and Init. The top selector uses alphabetized category submenus with program IDs unchanged. PATCH has a scrollable category sidebar with counts, an alphabetical sound list, search, favourites, previous/next, Import and Save As. Metadata fields are hidden until Edit details is opened. The download omits development logs, screenshots, Logic test projects, redundant exports and preview audio; full corresponding source and JUCE dependency sources are retained in one nested source ZIP.

## Preset selection flow — 0.3.1
The top preset selector is now a Browse button that opens the category browser directly, replacing the nested popup. Category buttons filter sounds; clicking a named row loads it. Accessible rows expose preset names instead of generic row numbers. The result label explicitly explains that a sound must be clicked to load. The main preset name follows the loaded processor state. Earlier category-menu tests only checked menu inventory; the updated regression test enters through the real editor's Browse button and checks that selecting EDM Arena Brass changes both preset metadata and its 900 Hz cutoff. No claim is made that the previous popup failure's exact cause was reproduced.
