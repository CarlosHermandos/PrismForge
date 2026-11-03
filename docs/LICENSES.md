# Dependency and content provenance

Local dependency inspected and used: JUCE 9.0.5 (`~/Documents/JUCE`). Its LICENSE.md identifies dual AGPLv3 / commercial JUCE licensing. The full third-party inventory is JUCE.spdx.json. Both notices/inventory are copied into the local package. The included Steinberg VST3 SDK retains its own license terms (see JUCE's SDK LICENSE.txt and SPDX inventory). No dependency was downloaded, and no commercial JUCE entitlement was assumed.

The project's source, panel layout, mathematical wavetables, procedural sample/multisample recipes, analytic factory spectral magnitudes, preset parameter recipes and generated WAV examples were created for Prism Forge. `Source/FactoryAssets.h` and `Parameters.h` define the original factory material. White/colored noise is generated with the fixed-seed engine. No commercial synth samples, presets, interface images, fonts or branding are bundled. Fonts use the system/JUCE stack.

Imported source material remains the user's content. User preset export embeds that material rather than depending on absolute external paths; redistribute it only with suitable rights. Factory presets do not depend on user imports.

The owner selected AGPLv3 on 2026-11-03. Project-owned source and original factory content are distributed under AGPL-3.0-only; see LICENSE.md and NOTICE.md. JUCE is used under its AGPLv3 option. The package includes the dependency source needed by the CMake build, its notices and SPDX inventory. Apple SDK/system frameworks and standard build tools must be supplied by the recipient. This records the chosen licensing route and provenance, not legal clearance of the design or a trademark/patent search. No notarization or Developer ID signing certificate is supplied.
