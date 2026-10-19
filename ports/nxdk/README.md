# Slime Volley on original Xbox

Private NXDK test port with a completed first hardware play test. See
[the play-test report](TESTING.md) for scope and performance limitations.
Keep the upstream C game and surface renderer; isolate
Xbox compatibility in this directory. Hardware results are bound to an exact
XBE hash in `test-results/acceptance.json`, not inferred from compilation.

## Scope and implementation

- Four player slots, two per side, using the existing local/AI selection logic.
  The desktop build still supports three per side. Default play remains 1v1.
- Existing physics, scoring, AI, game speeds, multiball, skins and themes.
- Original 800x600 logical playfield, scaled to 640x480 output by SDL2 surfaces.
- Select `Controller 1`–`Controller 4` on the team screen to assign physical
  Xbox ports 1–4. Menus accept any controller.
- D-pad/either stick move; left/right triggers also move left/right.
  A or Up on the D-pad/either stick jumps; X or Down changes skin.
  Opposing directions cancel; repeated controls in the same direction combine. A selects,
  B/Back returns, Start pauses/resumes. Hold Back+Start two seconds to reboot.
- `Options > Controls` displays Xbox button mappings and player-port help.
  Xbox Options omits the desktop fullscreen toggle.
- `Exit / System...` (or Back/B at the main menu) offers Cancel, Return to
  launcher, Restart game, Reboot Xbox and Power off Xbox. Cancel is selected
  initially in both the menu and each confirmation. Back/B cancels; an action
  requires selecting its confirmation and pressing A.
- This private console build returns directly to `E:\Dashboard\default.xbe`,
  verified against the running launcher before implementation. A missing
  launcher displays an error without exiting. Restart game reloads
  the running executable by its full native Xbox path; Reboot Xbox uses the normal firmware boot path; Power off
  saves preferences and shuts down through the Xbox SMC. All four actions
  pass through the game's normal audio/SDL/settings cleanup first.
- Existing settings save to `D:\.slime` when quitting normally. This preserves
  the upstream preference format/scope: theme, sound, display flags and bindings;
  it does not add persistence for team choices, speed or ball count.
- Networking and localization are disabled in the Xbox build. Desktop CMake
  configuration is preserved.

`Compat.h` is force-included only for game objects. It adapts the SDL 1.2 names
actually used by the game to NXDK's SDL2. Fonts and images use NXDK's bundled
SDL_ttf/FreeType and SDL_image; no asset replacement or graphics rewrite.
`Platform.c` handles presentation, controller polling, paths, theme directory
iteration, diagnostics and dashboard return. Gameplay input is applied during
legacy event polling without changing the game loop.

The small changes in `src/` are the Xbox player limit, removal of hard-coded
six-player indices, a controls-help dispatch, and audio correctness fixes:
initialize a valid format, clear the callback buffer, avoid pausing inside the
callback, synchronize playback changes, retain converted sound lengths, and
stop/close audio before freeing sound resources. The physics/AI files are
otherwise unchanged.

Startup has a 30-second timeout and shutdown a 15-second timeout. These write
`D:\watchdog.log` and request reboot; a watchdog recovery is a failed test,
not successful startup/shutdown. Recovery from a kernel fault is not guaranteed.

## Build

Host used: Apple Silicon macOS, Homebrew LLVM, GNU-compatible make, bison, flex,
Python 3, Git and CMake. No proprietary XDK is used. Use a path without spaces.

```sh
ports/nxdk/tools/bootstrap-sdk.sh
ports/nxdk/tools/build.sh
python3 ports/nxdk/tools/package.py
```

SDK: XboxDev/nxdk `c5c574c6ba5778595b782064044ff56b407e835b`, recursive
submodules, with the two patches in `patches/`. The initial SDK copy came from
the local Super Mario War port; it is isolated under ignored `.local/nxdk`.
The patches preserve that reference's deterministic XBE timestamp support and
bounded SDL audio-thread shutdown wait. They are recorded, not silently assumed.

`SOURCE_DATE_EPOCH` defaults to the game HEAD commit timestamp and is passed to
the linker and patched cxbe. The package records individual source hashes,
SDK/submodule revisions, patch paths, and the exact XBE hash. Uncommitted source
is identified by its content hashes. A clean SDK/game rebuild was compared
byte-for-byte; see the hardware acceptance record.

Outputs:

- `ports/nxdk/bin/default.xbe`
- `artifacts/slimevolley-nxdk-test.zip` — executable, original assets, notices,
  build identity and SHA-256 file manifest.
- `artifacts/slimevolley-nxdk-source.zip` — game/port source and assets, with
  the pinned SDK bootstrap instructions and patches.

The archive is a private candidate, not a claim of release qualification or
complete third-party source/license review for public distribution.

## Verification and deployment

```sh
ports/nxdk/tools/test-host.sh
python3 ports/nxdk/tools/xbox.py status
python3 ports/nxdk/tools/xbox.py deploy --label candidate-install
python3 ports/nxdk/tools/xbox.py launch --label candidate-launch
python3 ports/nxdk/tools/capture.py logs/hardware/menu.jpg
node ports/nxdk/tools/kvm-button.mjs http://192.168.1.169 a 200
```

Host tests execute the actual game initialization and audio code under address
and undefined-behavior sanitizers for desktop and Xbox player counts. They
exercise team sizes/skin selection, callback silence and converted buffer length;
they are not an Xbox emulator or a full desktop gameplay test. The host test
expects Homebrew SDL2; `DYLD_LIBRARY_PATH` makes its SDL3 compatibility backend
resolvable on this machine. Unrelated original compiler warnings are retained.

FTP defaults: `192.168.1.254`, `/F/SlimeVolley-nxdk-test`, dashboard credentials
`xbox/xbox`; override with `--host`, `--directory`, `XBOX_FTP_USER` and
`XBOX_FTP_PASSWORD`. Deploy preserves prior logs/settings/executable and reads
back every packaged file. Launch requires a completed deployment record and
rechecks every installed file before issuing XBMC `SITE RunXBE`.

After normal game Quit, XBMC may take some time to finish starting its FTP
service. Wait for readiness before collecting logs or deploying again:

```sh
python3 ports/nxdk/tools/xbox.py wait
python3 ports/nxdk/tools/xbox.py logs --label completed-run
```

The still-image API can return a cached frame after a video-mode change. The
capture tool instead takes a later frame from a bounded MJPEG stream. Retain
screenshots and logs; a successful button send or launch timeout is not proof
of the resulting game action.

Normal game exit writes preferences, closes audio, releases SDL, and reboots
to the dashboard. Do not overwrite failure evidence before collecting it.
`slime-error.log` may contain the original PNG assets' `iCCP` profile warnings;
these alone are not a loading failure.

## Remaining qualification

Four simultaneous human controllers, all physical ports/reconnect permutations,
physical stock-64-MB hardware, repeated cold starts, PAL/other video modes,
long-session stability, all theme/speed/multiball combinations and human audio
listening are separate checks. The XBE 64 MB flag is set; this is not by itself
proof of a 64 MB memory budget on an upgraded console.

Reference: the local Super Mario War project's NXDK build, FTP readback,
exact-hash acceptance and KVM workflow. `tools/kvm-button.mjs` is copied from
the local ESP-KVM project's Apache-2.0 bounded input helper.
