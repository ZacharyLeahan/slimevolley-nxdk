#!/usr/bin/env python3
"""Retain a later frame from a short MJPEG stream, avoiding stale still caches."""
import argparse,json,socket,time,urllib.request
from pathlib import Path
p=argparse.ArgumentParser(description=__doc__)
p.add_argument('output',type=Path);p.add_argument('--url',default='http://192.168.1.169');a=p.parse_args()
start=time.monotonic();buf=bytearray();frames=[]
with urllib.request.urlopen(a.url+'/stream',timeout=6) as r:
    while time.monotonic()-start<5:
        try:
            block=r.read(4096)
        except socket.timeout:
            # Static menus may stop producing MJPEG frames. Retain a complete
            # frame already received; absence of any frame still fails below.
            break
        if not block:break
        buf.extend(block)
        while True:
            first=buf.find(b'\xff\xd8');last=buf.find(b'\xff\xd9',first+2) if first>=0 else -1
            if first<0 or last<0:break
            frames.append(bytes(buf[first:last+2]));del buf[:last+2]
        if len(frames)>=3:break
if not frames:raise RuntimeError('No complete stream frame received; no visual acceptance')
a.output.parent.mkdir(parents=True,exist_ok=True);a.output.write_bytes(frames[-1])
a.output.with_suffix('.json').write_text(json.dumps({'frames_received':len(frames),'elapsed_seconds':time.monotonic()-start,'url':a.url+'/stream','freshness_note':'A static screen may yield one frame; assess freshness using observed screen transitions, not frame count alone.'},indent=2)+'\n')
print(a.output,'stream frames:',len(frames))
