# Prism Forge 0.3.1 open-source beta — 2026-11-03

macOS arm64: AU, VST3 and standalone. AGPL-3.0-only; full project and JUCE dependency source are included. License and notices are accessible from the plug-in header.

The EDM expansion adds 48 original patches (eight each in Brass, Bass, Leads, Pads, SFX and Chords), bringing the bank to 112 sounds plus Init. Program indices 0–64 are preserved. See EDM-BANK.md.

This update adds a distinct aubergine/lavender/amber presentation with flat hexagonal knob detail and a prism mark. Sound parameters and plug-in identifiers are unchanged, preserving existing session compatibility.

Validated: eight automated suites (AU instantiation requires execution outside the sandbox), strict Apple AU validation, all-parameter AU binary-plist state recall, and real Logic project save/close/reopen with Foundation plus an edited cutoff. See VALIDATION.md and logic-smoke.md for exact evidence. No manual sound-quality approval is claimed.

Install: quit Logic, extract the ZIP and run Install AU.command. Previous Prism Forge installations are preserved in the extracted package's Backups folder. In Logic choose AU Instruments > Prism Forge Audio > Prism Forge > Stereo. The standalone and VST3 bundles are included separately.

The build uses local ad-hoc signatures and is not Developer ID signed or notarized. Internet downloads may be blocked by Gatekeeper. This delivery is suitable for beta evaluation, not a claim of a complete commercial release. No IP clearance, brand-name clearance or patent search has been performed. Known DSP quality limits and deferred functionality remain documented in STATUS.md.

Rebuild: open a terminal in Source Project and follow README.md. The bundled ThirdParty/JUCE directory is selected automatically. Xcode with the macOS SDK, CMake 3.22+ and standard Python 3 are required. Rebuilt binaries need not be byte-identical due to toolchain/signing metadata.

0.3.1 replaces the top preset popup with direct access to the category browser. Click the preset name, choose a category, then click a sound. Preset rows now expose their names to accessibility tools. Factory sounds, parameters, saved-state schema and host program numbers are preserved.
