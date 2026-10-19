# Controller audit — 2026-10-19

The pinned nxdk SDL Xbox backend reports axes as LX, LY, LT, RX, RY, RT.
The port now shares a tested decoder between the Xbox build and host tests.

| Action | Inputs |
| --- | --- |
| Left / right | D-pad, either stick, left / right trigger |
| Jump | A, D-pad Up, either stick Up |
| Change skin | X, D-pad Down, either stick Down |
| Confirm | A or Start |
| Return | B or Back |
| Pause / resume | Start |
| Recovery reboot | Hold physical Back + Start for two seconds |

Stick deadzone is 12000 on signed 16-bit axes. Triggers activate above the
signed midpoint (roughly half travel); initial zero values cannot cause movement.
Duplicate directions combine; opposing directions cancel. Y, black, white and
stick clicks have no assigned action. Menus accept connected controllers;
gameplay retains each player's selected physical port.

The audit also fixed recovery mistakenly accepting B + Start because B and
Back share the normal navigation action. Recovery now checks physical buttons.

Validation: host ASan/UBSan core tests and decoder regression tests passed;
the nxdk build passed. The deployed executable and final FTP readback both hash
`e77e3e44aa49f72159f9decc64ee16551b21579402c1a9d041864b7f69111efe`.
KVM inputs exercised right-stick directions, both triggers separately/together,
conflicting sticks and B + Start held 2.3 seconds. Runtime masks confirmed input
recognition; B + Start did not reboot. The new Controls screen was captured from
the Xbox. Some MJPEG frames were stale or missing, so these captures do not
independently prove every gameplay effect. Decoder tests cover the aliases,
deadzones, diagonals, duplicate releases, opposing directions and recovery chord.
The final log records normal launcher exit and completed SDL/audio shutdown.
Raw evidence is retained locally under `logs/hardware/controller-*`.

Four simultaneous physical controllers, hotplug permutations, analog drift on
multiple retail pads and every menu/screen remain unverified.

## Last-pressed follow-up

Opposing directions now prioritize the latest newly pressed control. Releasing
that direction falls back to any still-held opposite. Each input source has
its own press history, including duplicate directions across sticks and triggers.
Exact simultaneous opposites keep the previous winner, or stay neutral if no
winner exists. Disconnect clears the port history. Host regression tests cover
trigger reversal/release, cross-control duplicate presses, vertical reversal,
held-input stability and simultaneous opposites.

The follow-up Xbox build SHA-256 is
`d8615e0864af258ea18bba71510b3cdab1406c2742153278d52eda84c73df4e0`.
This supersedes the neutral-opposites behavior tested above.
