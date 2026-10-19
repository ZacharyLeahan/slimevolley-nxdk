# Exit / System menu — 2026-10-19

Current build SHA-256:
`063280419a9b040625ff908394f27e3d245bf4950874b0b41f185a3f2208372c`.
The main menu exit entry and Back/B open Exit / System with Cancel selected.
Return to launcher, Restart game, Reboot Xbox and Power off Xbox each open a
separate confirmation with Cancel selected. Repeated Back cancels without
exiting, and A on Cancel keeps the game running.

On the final build, direct return to `E:\Dashboard\default.xbe`, reload of the
running executable by its native Xbox path, and normal console reboot all
passed on hardware. Runtime history records cleanup before actions 1, 2 and 3;
normal audio/SDL shutdown and executable readback were verified. The launcher
was identified by matching its executable against the running dashboard.
Power off was selected and confirmed; both Xbox and KVM became unreachable.
The user subsequently reported the console was back on and that a longer
play session "played great" (duration unspecified). This is user-reported
recovery and play feedback, not an independent LED measurement or instrumented
soak. Final log retrieval subsequently succeeded from the launcher. Persistent
history confirms `action=4 cleanup=complete` before power-off, followed by a
later clean return-to-launcher action. Executable readback matches the current
build hash; no watchdog log was present. Stderr contained only the known PNG
iCCP profile warnings. All four system actions now have recorded hardware
results, with physical power-on recovery reported by the user.

An early development build's reload through the temporary D: mapping produced
a black screen and needed a user power-cycle. The final native-path reload
passed. Launcher validation also required mounting E: explicitly; final direct
launcher return passed. These failures and exact evidence are preserved in
`test-results/system-menu-20261019.json`. Host sanitizer regressions, desktop
menu syntax, NXDK build and diff checks passed. This test covers the current
console and Modern theme, not every BIOS/dashboard installation.

# Xbox UI audit — 2026-10-19

Current UI build SHA-256:
`e05302d766a914088ef041a38cfe6e56f44ba34de0ca9d33bb9ddd1b124d486a`.
The Xbox menus use Controller 1–4 and controller-specific help, omit the desktop
fullscreen toggle, and label the normal exit Dashboard. Speed/theme help was
clarified. Mandatory players now offer AI levels 0–3 before cycling back to a
controller; returning to AI starts at level 0.

The build and both host sanitizer variants passed, including player-selection
regressions. Desktop menu syntax was checked using SDL 1.2 video declarations
with the host SDL2 headers; this is not a desktop runtime test. All 76 packaged
files were verified by Xbox readback. Main menu, Options, controls help, player
help, disabled-teammate help and AI levels 1 and 3 were inspected on hardware
in the Modern theme at 640x480. No clipping was observed; B returned from help,
and left Controller 1 was restored. This UI pass does not replace gameplay
soak qualification or multi-controller testing. Exact evidence is recorded in
`test-results/ui-audit-20261019.json`. The game is left at the main menu.

# Freeze investigation — 2026-10-19

A later manual gameplay session froze after approximately one minute. Its
preserved log stops during gameplay without normal shutdown, with stable
sampled available memory and no watchdog log. The earlier acceptance below
therefore does not establish long-session stability.

Both frame limiters in `src/jeu.c` read `SDL_GetTicks()` separately for the
comparison and the delay calculation. Crossing the deadline between those
reads can wrap the unsigned delay to 4,294,967,295 ms (almost 49.7 days).
The corrected code samples once and clamps overdue frames to zero delay.
Desktop and Xbox-count host sanitizer tests pass, including frame-deadline
and tick-wraparound regression checks. This is a concrete defect consistent
with the symptom; no stack trace proves it caused this specific hardware hang.

Corrected XBE SHA-256:
`950bd55cac49505dcae166433cf043941faf9b74b6caabf05552e65fe15e3e01`.
Deployment to the test Xbox verified all 76 packaged files by readback.
The corrected build completed 327.9 seconds of repeated active controller
input in 1v1 normal-speed, single-ball matches against level-0 AI. Movement,
jumping, skin changes, scoring, match-end/restart transitions and pause/resume
worked; no gameplay freeze was observed. The human player was explicitly
verified on key set A / controller port 1. Several MJPEG captures failed when
the KVM changed to H.264; its compatible still-image API confirmed continuing
gameplay around four and five minutes. A final MJPEG frame and Paused overlay
confirmed responsiveness after the active soak. This short test does not prove
indefinite stability or conclusively identify the original hang's cause.
Normal exit recorded `audio closed` and `SDL shutdown complete`, with no
watchdog log; executable readback matched the corrected hash. Stderr contained
only the known PNG profile warnings. Evidence and status are in
`test-results/freeze-investigation-20261019.json`.

# Hardware play test — 2026-10-19

Executable SHA-256:
`a5d0010cbfdf9fc4d29012cb1a9b585249d02d2f5c39bcfa955837313e86e1c4`.
The Xbox executable was unchanged during this play test. Results and exact
artifact/evidence hashes are recorded in `test-results/gameplay-20261019.json`.

The final build was played on the connected original Xbox through the ESP KVM.
D-pad and left-stick movement visibly changed the human slime's position; A
produced a jump; X changed its skin. A two-versus-two configuration with four
local assignments rendered and accepted the KVM controller. A further 2v2
session used one human-controlled slime and three level-0 AI players; rallies,
collisions and scoring continued. Start displayed the Paused overlay and a
second press resumed play. Returning from a match restored the main menu.

A separate 1v1 match reached the 0–10 results screen. The human slime was
intentionally moved away from the ball to exercise repeated scoring and the
match-end path predictably. This is functional acceptance, not a competitive
play or AI-strength assessment. A dismissed the results back to the main menu.
A twelve-second HDMI audio recording through the KVM was complete, contained
nonzero samples in both channels, and reported no capture overruns. Human
listening and source waveform matching were not performed.

Initial KVM USB/network interruptions prevented input; after its USB connection
returned, the already-running game accepted controller input without a new XBE.
A static screen can yield only one MJPEG frame because the KVM skips unchanged
images. The capture helper now preserves that frame and records the limitation;
observed screen transitions establish the input results, not frame count alone.

Raw screenshots/audio/runtime logs remain under `logs/hardware/play-final-*`.
Four simultaneous human controllers, every physical-port/reconnect permutation,
physical retail-64-MB hardware, repeated cold starts, all themes/modes and a long
soak remain outside this first play test.

Normal exit completed: the post-game executable readback matched the tested
hash, the log recorded `audio closed` and `SDL shutdown complete`, and XBMC
returned. Stderr contained only the shipped PNGs' known iCCP-profile warnings.
Four-slot presentation samples (36 samples with at least 10 FPS) ranged
25.1–31.8 FPS, median 31.7. These include pauses/transitions and are not a
controlled benchmark; the intended 50 FPS has not been reached. At least
52,572 KiB available memory was sampled during that four-slot subset on the
upgraded console; this does not establish a stock-64-MB allocation budget.

One subsequent warm relaunch returned to the main menu; the Xbox is left there
ready for manual play. This is one relaunch, not repeated-start qualification.
