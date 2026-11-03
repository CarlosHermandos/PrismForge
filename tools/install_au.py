#!/usr/bin/env python3
"""Back up the installed Prism Forge AU, install the local build, optionally run auval."""
from pathlib import Path
import argparse
import os
import plistlib
import shutil
import subprocess
import uuid

parser = argparse.ArgumentParser()
parser.add_argument('--validate', action='store_true')
args = parser.parse_args()
project = Path(__file__).resolve().parents[1]
source = project / 'build/PrismForge_artefacts/Release/AU/Prism Forge.component'
identifier = 'audio.prismforge.instrument'

def bundle_id(path):
    with (path / 'Contents/Info.plist').open('rb') as stream:
        return plistlib.load(stream).get('CFBundleIdentifier')

if not source.is_dir() or bundle_id(source) != identifier:
    raise SystemExit('The expected Prism Forge AU build is missing.')
if subprocess.run(['/usr/bin/pgrep', '-x', 'Logic Pro'], stdout=subprocess.DEVNULL).returncode == 0:
    raise SystemExit('Quit Logic before updating the AU. No running session was changed.')
folder = Path.home() / 'Library/Audio/Plug-Ins/Components'
folder.mkdir(parents=True, exist_ok=True)
target = folder / source.name
if target.exists() and bundle_id(target) != identifier:
    raise SystemExit('Refusing to replace a component with a different bundle identifier.')
staging = folder / ('.PrismForge-' + uuid.uuid4().hex + '.component')
subprocess.run(['/usr/bin/ditto', str(source), str(staging)], check=True)
subprocess.run(['/usr/bin/codesign', '--verify', '--deep', '--strict', str(staging)], check=True)
backup = None
if target.exists():
    backups = project / 'Backups/installed-AU'
    backups.mkdir(parents=True, exist_ok=True)
    backup = backups / ('Prism Forge-' + uuid.uuid4().hex[:8] + '.component')
    os.rename(target, backup)
try:
    os.rename(staging, target)
except BaseException:
    if backup is not None and not target.exists():
        os.rename(backup, target)
    raise
print('Installed:', target, flush=True)
if backup:
    print('Previous build preserved:', backup, flush=True)
print('Reopen Logic to load the updated binary.', flush=True)
if args.validate:
    log = project / 'docs/auval.log'
    with log.open('w') as stream:
        result = subprocess.run(['/usr/bin/auval', '-strict', '-v', 'aumu', 'PrF1', 'PrFg'], stdout=stream, stderr=subprocess.STDOUT)
    print('auval exit:', result.returncode, '/ log:', log, flush=True)
    raise SystemExit(result.returncode)
