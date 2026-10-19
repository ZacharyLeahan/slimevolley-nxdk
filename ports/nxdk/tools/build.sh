#!/usr/bin/env bash
set -euo pipefail
root=$(cd "$(dirname "$0")/../../.." && pwd)
export NXDK_DIR="${NXDK_DIR:-$root/.local/nxdk}"
if [ ! -d "$NXDK_DIR" ]; then "$root/ports/nxdk/tools/bootstrap-sdk.sh"; fi
export SOURCE_DATE_EPOCH="${SOURCE_DATE_EPOCH:-$(git -C "$root" show -s --format=%ct HEAD)}"
export PATH="$NXDK_DIR/bin:/opt/homebrew/opt/llvm/bin:/opt/homebrew/opt/bison/bin:/opt/homebrew/opt/flex/bin:$PATH"
mkdir -p "$root/logs"
make -C "$root/ports/nxdk" -j"${JOBS:-4}" "$@" 2>&1 | tee "$root/logs/build.log"
