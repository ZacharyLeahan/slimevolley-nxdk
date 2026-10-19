#!/usr/bin/env bash
set -euo pipefail
root=$(cd "$(dirname "$0")/../../.." && pwd)
mkdir -p "$root/.local/tests"
if [ "$(uname -s)" = Darwin ]; then
    export PATH="/opt/homebrew/opt/llvm/bin:/opt/homebrew/bin:$PATH"
    linker=(-Wl,-dead_strip -Wl,-undefined,dynamic_lookup)
else
    linker=(-Wl,--gc-sections)
fi
cc=${HOST_CC:-clang}
read -r -a sdl_cflags <<< "$(pkg-config --cflags sdl2)"
read -r -a sdl_libs <<< "$(pkg-config --libs sdl2)"
for variant in desktop xbox; do
    flags=(-DSV_HOST_TEST=1)
    if [ "$variant" = xbox ]; then flags=(-DSV_XBOX=1); fi
    $cc -g -O1 -fcommon -ffunction-sections -fdata-sections \
      -fsanitize=address,undefined "${linker[@]}" \
      "${sdl_cflags[@]}" -I"$root/ports/nxdk/tests/include" -I"$root/src" \
      -include "$root/ports/nxdk/tests/HostCompat.h" "${flags[@]}" \
      "$root/ports/nxdk/tests/core.c" "$root/src/jeu.c" "$root/src/audio.c" "$root/src/menu_jeu.c" \
      "${sdl_libs[@]}" -o "$root/.local/tests/$variant"
    "$root/.local/tests/$variant"
done

$cc -g -O1 -fsanitize=address,undefined \
  "${sdl_cflags[@]}" "$root/ports/nxdk/tests/controller.c" -o "$root/.local/tests/controller"
"$root/.local/tests/controller"
