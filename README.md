# Slime Volley — nxdk Xbox port

Development fork of Slime Volley for a planned original Xbox port using
[nxdk](https://github.com/XboxDev/nxdk), the open-source Xbox development kit.

## Status

An NXDK test build now exists, with the original game core preserved and an
Xbox-specific limit of two players per side. See [the port documentation](ports/nxdk/README.md)
for builds, deployment, controls and exact-hash hardware test scope.

The Xbox target is local multiplayer with four independently assigned controller
slots. The desktop build retains the original six-player limit. Hardware tests
use the existing Xbox/ESP IP KVM workflow.

## Source and attribution

Slime Volley is by Côme Bernigaud (MCMic) and Vincent Duvert (VinDuv).
This repository preserves the available Debian-maintained Git history from
<https://salsa.debian.org/games-team/slimevolley>, starting from commit
`3548c5e` on its master branch. That repository includes the game's C/SDL source
and Debian packaging; it is not presented as the complete original development
history. The source repository remains configured as the `upstream` remote.

Original project: <https://slime.tuxfamily.org/>.
See [the original README](docs/README) for the existing game documentation.

## Licensing

Game code is GPL-3.0-or-later. Artwork, sounds, fonts, and packaging have their
own notices. Preserve these when modifying or distributing the game; see
[the detailed copyright and license notices](debian/copyright).
