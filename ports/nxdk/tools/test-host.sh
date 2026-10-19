#!/usr/bin/env bash
set -euo pipefail
root=$(cd "$(dirname "$0")/../../.." && pwd)
mkdir -p "$root/.local/tests"
for variant in desktop xbox; do
    flags=(-DSV_HOST_TEST=1)
    if [ "$variant" = xbox ]; then flags=(-DSV_XBOX=1); fi
    /opt/homebrew/opt/llvm/bin/clang -g -O1 -fcommon -ffunction-sections -fdata-sections \
      -fsanitize=address,undefined -Wl,-dead_strip -Wl,-undefined,dynamic_lookup \
      -I/opt/homebrew/include/SDL2 -I"$root/ports/nxdk/tests/include" -I"$root/src" \
      -include "$root/ports/nxdk/tests/HostCompat.h" "${flags[@]}" \
      "$root/ports/nxdk/tests/core.c" "$root/src/jeu.c" "$root/src/audio.c" "$root/src/menu_jeu.c" \
      -L/opt/homebrew/lib -lSDL2 -o "$root/.local/tests/$variant"
    DYLD_LIBRARY_PATH=/opt/homebrew/lib "$root/.local/tests/$variant"
done

/opt/homebrew/opt/llvm/bin/clang -g -O1 -fsanitize=address,undefined \
  -I/opt/homebrew/include/SDL2 "$root/ports/nxdk/tests/controller.c" -o "$root/.local/tests/controller"
"$root/.local/tests/controller"
