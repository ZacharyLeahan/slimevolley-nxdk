#!/usr/bin/env bash
set -euo pipefail
root=$(cd "$(dirname "$0")/../../.." && pwd)
sdk="$root/.local/nxdk"
if [ -e "$sdk" ]; then echo "SDK already exists: $sdk"; exit 0; fi
mkdir -p "$root/.local"
git clone https://github.com/XboxDev/nxdk.git "$sdk"
git -C "$sdk" checkout c5c574c6ba5778595b782064044ff56b407e835b
git -C "$sdk" submodule update --init --recursive
git -C "$sdk" apply "$root/ports/nxdk/patches/cxbe-source-date-epoch.patch"
git -C "$sdk/lib/sdl/SDL2" apply "$root/ports/nxdk/patches/sdl-audio-shutdown.patch"
