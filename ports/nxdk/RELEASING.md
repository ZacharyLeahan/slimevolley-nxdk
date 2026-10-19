# Alpha builds and pre-releases

The Xbox build workflow runs host sanitizer tests, checks out the pinned nxdk
SDK and our patches, compiles the XBE, and uploads testing downloads. It runs on
main, codex branches, pull requests and manually. No console credentials or KVM
connection are needed on GitHub. A successful build does not imply hardware acceptance.

## Get a testing build

Open **Actions → Xbox build**, select a successful run, and download its
`xbox-<commit>` artifact (GitHub sign-in required). Extract the artifact wrapper,
then extract `slimevolley-nxdk-test.zip`. Copy its complete contents to a separate
Xbox game folder and launch `default.xbe`; keep `data/` next to it.
The workflow retains these artifacts for 30 days. `SHA256SUMS.txt` checks the
binary and matching-source ZIPs; `BUILD-INFO.json` inside the binary ZIP records
the XBE hash, source identity, pinned SDK and compiler. `MANIFEST.json` checks
individual package files. GitHub's automatic source-code downloads contain source,
not a compiled Xbox game.

## Prepare a release

1. Merge the candidate to main.
2. Open **Actions → Xbox build → Run workflow** on main. Set the optional tag
   to `v0.1.0-alpha.1` (increment alpha.N for later previews).
3. The workflow builds, tests and creates a **draft pre-release** with the same
   ZIPs and checksums attached. It refuses stable version tags. A blank tag only
   builds downloads; draft creation only runs from main. Existing tags/releases
   are never overwritten automatically.
4. Download that exact draft package and test it on the Xbox. Record the XBE hash,
   controller/gameplay/system-menu results, hardware used and known limitations
   in the draft notes. If it fails, fix it and create a new candidate; do not
   substitute a different local build and call it tested.
5. In **Releases**, review the draft, replace the pending hardware-test paragraph,
   keep **pre-release** enabled, and publish. This is a manual action; the workflow
   never publishes a release. Published release assets provide the public game
   download without requiring people to build the project.

First-public-preview numbering is `v0.1.0-alpha.1`, not v1.0. Further testing
previews use alpha.2, alpha.3, etc. Reserve stable versioning for a later decision.

## Local builds

Linux prerequisites: clang, lld, llvm, GNU make, bison, flex, CMake, pkg-config,
SDL2 development headers, Python 3 and Git. Put the LLVM tools directory on PATH
when the distribution installs versioned tools (CI uses LLVM 18 on Ubuntu 24.04).
macOS uses Homebrew LLVM, bison, flex, SDL2 and pkg-config. The host tests accept
`HOST_CC`; packaging records that compiler (or the platform default). Build with
`bash ports/nxdk/tools/build.sh`, then `python3 ports/nxdk/tools/package.py`.
