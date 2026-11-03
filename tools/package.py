#!/usr/bin/env python3
"""Build the clean end-user download with corresponding source in one nested archive."""
from pathlib import Path
import csv
import hashlib
import shutil
import subprocess
import tempfile
import uuid
import zipfile

project = Path(__file__).resolve().parents[1]
root = project / 'dist/Prism Forge 0.3.1 macOS arm64'
if root.exists():
    backup = project / 'Backups/packages' / uuid.uuid4().hex[:8]
    backup.parent.mkdir(parents=True, exist_ok=True)
    root.rename(backup)
root.mkdir(parents=True)
artifacts = project / 'build/PrismForge_artefacts/Release'
for format, extension in [('AU', 'component'), ('VST3', 'vst3'), ('Standalone', 'app')]:
    destination = root / format / ('Prism Forge.' + extension)
    destination.parent.mkdir(parents=True, exist_ok=True)
    subprocess.run(['/usr/bin/ditto', str(artifacts / format / destination.name), str(destination)], check=True)
    subprocess.run(['/usr/bin/codesign', '--verify', '--deep', '--strict', str(destination)], check=True)
with (project / 'docs/vst3-results.log').open('w') as result_log:
    subprocess.run([str(project / 'build/PrismVST3Smoke'), str(root / 'VST3/Prism Forge.vst3')], stdout=result_log, stderr=subprocess.STDOUT, check=True)
cache = (project / 'build/CMakeCache.txt').read_text()
juce = Path(next(line.split('=', 1)[1] for line in cache.splitlines() if line.startswith('JUCE_PATH:PATH=')))
licenses = root / 'Licenses'
licenses.mkdir()
for source, name in [(project / 'LICENSE.md', 'LICENSE.md'), (project / 'NOTICE.md', 'NOTICE.md'), (juce / 'LICENSE.md', 'JUCE-LICENSE.md'), (juce / 'JUCE.spdx.json', 'JUCE.spdx.json'), (juce / 'modules/juce_audio_processors_headless/format_types/VST3_SDK/LICENSE.txt', 'VST3-SDK-LICENSE.txt')]:
    shutil.copy2(source, licenses / name)
source_folder = root / 'Source'
source_folder.mkdir()
with tempfile.TemporaryDirectory(prefix='prism-source-') as staging:
    source_root = Path(staging) / 'Prism Forge Source'
    source_root.mkdir()
    for folder in ['Source', 'Tests', 'tools']:
        shutil.copytree(project / folder, source_root / folder, ignore=shutil.ignore_patterns('__pycache__', '.DS_Store'))
    for name in ['CMakeLists.txt', 'README.md', 'LICENSE.md', 'NOTICE.md', '.clang-format', '.gitignore']:
        shutil.copy2(project / name, source_root / name)
    docs = source_root / 'docs'
    docs.mkdir()
    for file in (project / 'docs').glob('*.md'):
        shutil.copy2(file, docs / file.name)
    shutil.copytree(juce, source_root / 'ThirdParty/JUCE', ignore=shutil.ignore_patterns('.git', '.DS_Store', 'build', 'cmake-build-*', '__pycache__'))
    with zipfile.ZipFile(source_folder / 'PrismForge-0.3.1-Source.zip', 'w', zipfile.ZIP_DEFLATED) as source_zip:
        for file in sorted(source_root.rglob('*')):
            if file.is_file(): source_zip.write(file, file.relative_to(Path(staging)))
(root / 'START HERE.txt').write_text("""PRISM FORGE 0.3.1 / macOS Apple Silicon / OPEN-SOURCE BETA

LOGIC PRO
1. Quit Logic.
2. Run Install AU.command. Your previous Prism Forge component is backed up.
3. Reopen Logic. Add a software instrument track and choose:
   AU Instruments > Prism Forge Audio > Prism Forge > Stereo.

PRESETS
112 sounds + Init are already built in; no preset import is needed.
Open PATCH for category buttons, search and favourites.
Click the preset name at the top to open the same browser.
Use Edit details only when editing preset metadata.

OTHER FORMATS
Standalone/Prism Forge.app: choose your audio/MIDI devices in Options.
VST3/Prism Forge.vst3: for compatible hosts; Logic uses the AU instead.

SOURCE AND LICENSE
AGPL-3.0-only. Licenses/ contains the license and notices.
Source/PrismForge-0.3.1-Source.zip contains project and dependency source,
build instructions, tests and technical documentation. It is not an installer.
Keep source and notices with this download when redistributing.

This beta is locally ad-hoc signed, not notarized. Manual sound-quality approval
and some DSP/host QA remain open; see source docs/STATUS.md for limitations.
""")
installer = root / 'Install AU.command'
installer.write_text('''#!/bin/zsh
set -euo pipefail
prism_dir=${0:A:h}
if /usr/bin/pgrep -x 'Logic Pro' >/dev/null; then
  echo 'Quit Logic before installing. No project has been closed or modified.'
  exit 1
fi
prism_components="$HOME/Library/Audio/Plug-Ins/Components"
prism_backups="$prism_dir/Backups"
prism_uuid=$(/usr/bin/uuidgen)
prism_stage="$prism_components/.PrismForge-$prism_uuid.component"
prism_target="$prism_components/Prism Forge.component"
/bin/mkdir -p "$prism_components" "$prism_backups"
if [[ -e "$prism_target" ]]; then
  prism_id=$(/usr/libexec/PlistBuddy -c 'Print :CFBundleIdentifier' "$prism_target/Contents/Info.plist")
  if [[ "$prism_id" != 'audio.prismforge.instrument' ]]; then
    echo 'Refusing to replace a different instrument.'
    exit 1
  fi
fi
/usr/bin/ditto "$prism_dir/AU/Prism Forge.component" "$prism_stage"
/usr/bin/codesign --verify --deep --strict "$prism_stage"
if [[ -e "$prism_target" ]]; then
  /bin/mv "$prism_target" "$prism_backups/Prism Forge-$prism_uuid.component"
fi
/bin/mv "$prism_stage" "$prism_target"
echo 'Prism Forge installed. Previous build preserved in Backups. Reopen Logic.'
''')
installer.chmod(0o755)
subprocess.run(['/bin/zsh', '-n', str(installer)], check=True)
(root / 'SHA256SUMS.txt').write_text('\n'.join(hashlib.sha256(file.read_bytes()).hexdigest() + '  ' + str(file.relative_to(root)) for file in sorted(root.rglob('*')) if file.is_file() and file.name != 'SHA256SUMS.txt') + '\n')
archive = project / 'dist/PrismForge-0.3.1-macOS-arm64.zip'
subprocess.run(['/usr/bin/ditto', '-c', '-k', '--norsrc', '--noextattr', '--keepParent', str(root), str(archive)], check=True)
print(root)
print(archive)
print('Archive bytes:', archive.stat().st_size)

# Portable EDM-only bank, organized by category for individual PATCH > Import.
import zipfile
edm_archive = project / 'dist/PrismForge-EDM-48-Presets.zip'
with zipfile.ZipFile(edm_archive, 'w', zipfile.ZIP_DEFLATED) as bank:
    with (project / 'Content/Presets.tsv').open() as stream:
        rows = [row for row in csv.DictReader(stream, delimiter='\t') if 'edm' in row['tags'].split(',')]
    assert len(rows) == 48
    for row in rows:
        file = project / 'Content/Factory Presets' / (row['name'] + '.prism')
        bank.write(file, row['category'] + '/' + file.name)
    bank.write(project / 'docs/EDM-BANK.md', 'EDM-BANK.md')
    bank.write(project / 'LICENSE.md', 'LICENSE.md')
    bank.write(project / 'NOTICE.md', 'NOTICE.md')
    bank.writestr('README.txt', 'PRISM FORGE / 48 ORIGINAL EDM PRESETS\n\nThe updated plugin already includes these sounds in PATCH.\nFor a portable file, choose PATCH > Import and select a .prism file.\nCategories: Brass, Bass, Leads, Pads, SFX and Chords (8 each).\nChords play fixed intervals from one note; risers need a held note.\nRequires Prism Forge; these files are not Serum presets.\nAGPL-3.0-only original parameter recipes. See EDM-BANK.md and LICENSE.md.\n')
print(edm_archive)
for file in [archive, edm_archive]:
    file.with_suffix(file.suffix + '.sha256').write_text(hashlib.sha256(file.read_bytes()).hexdigest() + '  ' + file.name + '\n')
