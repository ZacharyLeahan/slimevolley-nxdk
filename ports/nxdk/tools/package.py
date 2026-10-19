#!/usr/bin/env python3
"""Package a private Xbox test build, retaining exact source/dependency identity."""
import hashlib,json,os,shutil,subprocess,zipfile
from pathlib import Path
from datetime import datetime,timezone
ROOT=Path(__file__).resolve().parents[3]
def git(path,*args):
    return subprocess.check_output(['git','-C',str(path),*args],text=True).strip()
def digest(path):return hashlib.sha256(path.read_bytes()).hexdigest()
out=ROOT/'artifacts/slimevolley-nxdk-test'
if out.exists():shutil.rmtree(out)
out.mkdir(parents=True)
shutil.copy2(ROOT/'ports/nxdk/bin/default.xbe',out/'default.xbe')
shutil.copytree(ROOT/'data',out/'data',ignore=shutil.ignore_patterns('CMakeLists.txt'))
(out/'licenses').mkdir()
for name,source in [('GAME-LICENSE.txt','docs/LICENSE'),('ASSET-NOTICES.txt','debian/copyright')]:
    shutil.copy2(ROOT/source,out/'licenses'/name)
sdk=Path(os.environ.get('NXDK_DIR',str(ROOT/'.local/nxdk')))
for name,source in [('SDL.txt',sdk/'lib/sdl/SDL2/LICENSE.txt'),('SDL_image.txt',sdk/'lib/sdl/SDL2_image/COPYING.txt'),('SDL_ttf.txt',sdk/'lib/sdl/SDL_ttf/COPYING.txt'),('FreeType.txt',sdk/'lib/sdl/SDL_ttf/external/freetype-2.4.12/docs/FTL.TXT'),('FreeType-GPL.txt',sdk/'lib/sdl/SDL_ttf/external/freetype-2.4.12/docs/GPLv2.TXT')]:
    if source.exists():shutil.copy2(source,out/'licenses'/name)
source_files=sorted([*ROOT.glob('src/*.[ch]'),*ROOT.glob('ports/nxdk/**/*.py'),*ROOT.glob('ports/nxdk/**/*.sh'),*ROOT.glob('ports/nxdk/**/*.mjs'),*ROOT.glob('ports/nxdk/**/*.h'),*ROOT.glob('ports/nxdk/**/*.c'),ROOT/'ports/nxdk/Makefile',*ROOT.glob('ports/nxdk/patches/*'),*ROOT.glob('.github/workflows/*.yml')])
source_manifest={str(f.relative_to(ROOT)):digest(f) for f in source_files}
info={'created_utc':datetime.now(timezone.utc).isoformat(),'game_commit':git(ROOT,'rev-parse','HEAD'),'source_files':source_manifest,'source_digest':hashlib.sha256(json.dumps(source_manifest,sort_keys=True).encode()).hexdigest(),'build_flags':(ROOT/'ports/nxdk/.build-config').read_text(),'compiler':subprocess.check_output([os.environ.get('HOST_CC','/opt/homebrew/opt/llvm/bin/clang' if Path('/opt/homebrew/opt/llvm/bin/clang').exists() else 'clang'),'--version'],text=True).strip(),'sdk_commit':git(sdk,'rev-parse','HEAD'),'sdk_submodules':git(sdk,'submodule','status','--recursive'),'sdk_patches':[str(p.relative_to(ROOT)) for p in ROOT.glob('ports/nxdk/patches/*')],'xbe_sha256':digest(out/'default.xbe'),'hardware_acceptance':'See ports/nxdk/test-results; compilation alone is not hardware acceptance.'}
(out/'BUILD-INFO.json').write_text(json.dumps(info,indent=2)+'\n')
(out/'README.txt').write_text('Slime Volley NXDK alpha testing preview\nCopy this entire folder to a separate Xbox directory and launch default.xbe.\nFour player slots, two per side; original AI and gameplay.\nD-pad/either stick move; LT left, RT right. Latest opposing press wins.\nA/Up jump, X/Down color, A select, B/Back return, Start pause.\nPlayers select physical Xbox ports 1-4.\nLogs: slime.log and slime-error.log. Preferences: .slime.\nExact-hash hardware results live in the source repository.\nOriginal authors: Come Bernigaud (MCMic), Vincent Duvert (VinDuv).\nSource: https://github.com/ZacharyLeahan/slimevolley-nxdk\n')
manifest={str(f.relative_to(out)):digest(f) for f in sorted(out.rglob('*')) if f.is_file()}
(out/'MANIFEST.json').write_text(json.dumps(manifest,indent=2)+'\n')
with zipfile.ZipFile(out.with_suffix('.zip'),'w',zipfile.ZIP_DEFLATED) as z:
    for f in sorted(out.rglob('*')):
        if f.is_file():z.write(f,str(f.relative_to(out)))
with zipfile.ZipFile(ROOT/'artifacts/slimevolley-nxdk-source.zip','w',zipfile.ZIP_DEFLATED) as z:
    for f in source_files:z.write(f,str(f.relative_to(ROOT)))
    for f in [ROOT/'CMakeLists.txt',ROOT/'config.h.cmake',ROOT/'README.md',*ROOT.glob('docs/*'),ROOT/'debian/copyright',*ROOT.glob('ports/nxdk/*.md'),*ROOT.glob('ports/nxdk/test-results/*.json'),*ROOT.glob('data/**/*')]:
        if f.is_file():z.write(f,str(f.relative_to(ROOT)))
print(json.dumps({'package':str(out.with_suffix('.zip')),'files':len(manifest)+1,'xbe_sha256':info['xbe_sha256'],'source_digest':info['source_digest']},indent=2))

checksums=[]
for f in [out.with_suffix('.zip'),ROOT/'artifacts/slimevolley-nxdk-source.zip']:
    checksums.append(f"{digest(f)}  {f.name}")
(ROOT/'artifacts/SHA256SUMS.txt').write_text('\n'.join(checksums)+'\n')
