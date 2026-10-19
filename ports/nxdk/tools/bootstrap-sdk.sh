#!/usr/bin/env bash
set -euo pipefail
root=$(cd "$(dirname "$0")/../../.." && pwd)
sdk="$root/.local/nxdk"
if [ -e "$sdk" ]; then echo "SDK already exists: $sdk"; exit 0; fi
mkdir -p "$root/.local"
git clone https://github.com/XboxDev/nxdk.git "$sdk"
git -C "$sdk" checkout 14d5ee97e73347c973f1f57b68b79ec08c9e77f2
git -C "$sdk" apply "$root/ports/nxdk/patches/ac97-ready-link.patch"
git -C "$sdk" submodule update --init --recursive
git -C "$sdk" apply "$root/ports/nxdk/patches/cxbe-source-date-epoch.patch"
git -C "$sdk/lib/sdl/SDL2" apply "$root/ports/nxdk/patches/sdl-audio-shutdown.patch"
