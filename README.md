# Slime Volley — nxdk Xbox port

Development fork of Slime Volley for a planned original Xbox port using
[nxdk](https://github.com/XboxDev/nxdk), the open-source Xbox development kit.

## Status

Source imported; Xbox porting has not started. No Xbox build or hardware
compatibility is claimed yet.

The intended scope is local multiplayer, including four independently controlled
player slots for repeatable hardware-in-the-loop testing on a real Xbox.

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
