#!/usr/bin/env python3
"""Bounded XBMC FTP deployment and exact readback verification."""
import argparse,hashlib,json,os,socket,time
from ftplib import FTP,error_perm
from pathlib import Path
from datetime import datetime,timezone
ROOT=Path(__file__).resolve().parents[3]
p=argparse.ArgumentParser(description=__doc__)
p.add_argument('action',choices=['status','deploy','launch','logs','wait'])
p.add_argument('--host',default='192.168.1.254')
p.add_argument('--directory',default='/F/SlimeVolley-nxdk-test')
p.add_argument('--label',default=datetime.now(timezone.utc).strftime('%Y%m%dT%H%M%SZ'))
a=p.parse_args()
evidence=ROOT/'logs/hardware'/a.label;evidence.mkdir(parents=True,exist_ok=True)
if a.action=='wait':
    deadline=time.monotonic()+60
    while time.monotonic()<deadline:
        try:
            with socket.create_connection((a.host,21),timeout=1) as probe:
                probe.settimeout(1)
                if probe.recv(1024).startswith(b'220'):print('Dashboard FTP ready');raise SystemExit(0)
        except OSError:pass
        time.sleep(2)
    raise SystemExit('Dashboard FTP did not return within 60 seconds')
f=FTP(a.host,timeout=8);f.login(os.getenv('XBOX_FTP_USER','xbox'),os.getenv('XBOX_FTP_PASSWORD','xbox'))
if a.action=='status':print(f.getwelcome(),f.nlst())
elif a.action=='deploy':
    package=ROOT/'artifacts/slimevolley-nxdk-test'
    manifest=json.loads((package/'MANIFEST.json').read_text())
    # Preserve saves and logs if revisiting an installed candidate.
    try:
        f.cwd(a.directory)
        for name in ['slime.log','slime-error.log','watchdog.log','system-actions.log','.slime','default.xbe']:
            try:
                with (evidence/('prior-'+name.lstrip('.'))).open('wb') as out:f.retrbinary('RETR '+name,out.write)
            except error_perm:pass
    except error_perm:pass
    def mkdirs(path):
        f.cwd('/')
        for part in path.strip('/').split('/'):
            try:f.cwd(part)
            except error_perm:f.mkd(part);f.cwd(part)
    checked={}
    for rel in [*manifest,'MANIFEST.json']:
        local=package/rel;expected=hashlib.sha256(local.read_bytes()).hexdigest()
        if rel in manifest and manifest[rel]!=expected:raise RuntimeError('Local package changed: '+rel)
        parent=Path(rel).parent.as_posix();mkdirs(a.directory+('' if parent=='.' else '/'+parent))
        with local.open('rb') as src:f.storbinary('STOR '+local.name,src,65536)
        h=hashlib.sha256();f.retrbinary('RETR '+local.name,h.update,65536)
        if h.hexdigest()!=expected:raise RuntimeError('Readback mismatch: '+rel)
        checked[rel]=expected
    result={'utc':datetime.now(timezone.utc).isoformat(),'host':a.host,'directory':a.directory,'verified_files':checked}
    (evidence/'deploy.json').write_text(json.dumps(result,indent=2)+'\n');print('Verified',len(checked),'files; XBE',checked['default.xbe'])
elif a.action=='logs':
    f.cwd(a.directory)
    for name in ['slime.log','slime-error.log','watchdog.log','system-actions.log','.slime','default.xbe']:
        try:
            with (evidence/name).open('wb') as out:f.retrbinary('RETR '+name,out.write)
        except error_perm as e:print(name,str(e))
    print(evidence)
else:
    # Require a completed deployment record and recheck the installed XBE before launch.
    records=list((ROOT/'logs/hardware').glob('*/deploy.json'))
    records=[json.loads(r.read_text()) for r in records]
    records=[r for r in records if r['host']==a.host and r['directory']==a.directory]
    if not records:raise RuntimeError('No completed, verified deployment for this target')
    latest=max(records,key=lambda r:r['utc']);f.cwd(a.directory)
    for rel,expected in latest['verified_files'].items():
        h=hashlib.sha256();f.retrbinary('RETR '+rel,h.update,65536)
        if h.hexdigest()!=expected:raise RuntimeError('Installed package changed: '+rel)
    path=a.directory.strip('/').replace('/','\\');path=path[0]+':'+path[1:]+'\\default.xbe'
    print('Launching',path,flush=True)
    try:print(f.sendcmd('SITE RunXBE '+path))
    except (EOFError,socket.timeout,ConnectionError) as e:print(type(e).__name__,'during launch; inspect KVM output.')
f.close()
