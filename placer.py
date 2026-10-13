# placer.py [obj]: retail slots (call targets in gaps) for obj's unmatched fns, best size fit; no arg = summary over all objs; run from target/
import json,struct,sys,collections
d=open('../../cod2engineold/binaries/windows/1.0/CoD2MP_s.exe','rb').read()
pe=struct.unpack_from('<I',d,0x3c)[0];oh=struct.unpack_from('<H',d,pe+20)[0];sec=pe+24+oh
n,vs,va,rs,ro=struct.unpack_from('<8sIIII',d,sec);tva=0x400000+va;t=d[ro:ro+vs]
tg=set()
for i in range(len(t)-5):
    if t[i]==0xe8:
        x=tva+i+5+struct.unpack_from('<i',t,i+1)[0]
        if tva<=x<tva+vs and x%16==0: tg.add(x)
E=json.load(open('build/exp.json'))
hit=sorted((int(e['hits'][0],16),e['size'],e['obj']) for e in E if len(e['hits'])==1)
known=[(a,a+s) for a,s,o in hit]
starts=sorted(set([a for a,b in known]+list(tg)))
import bisect
def slot(x):  # size from x to next known start or call target
    i=bisect.bisect_right(starts,x); return (starts[i] if i<len(starts) else x)-x
covered=lambda x: any(a<=x<b for a,b in known[max(0,bisect.bisect_right(known,(x,1<<40))-1):][:1])
def place(obj):
    mine=sorted((a,a+s) for a,s,o in hit if o==obj)
    if len(mine)<2: return []
    # main cluster: median +- 0x40000
    med=mine[len(mine)//2][0];mine=[m for m in mine if abs(m[0]-med)<0x40000]
    lo,hi=mine[0][0],mine[-1][1]
    # extend lo/hi over call targets not covered by any known fn of another obj
    prev=[a for a,b in known if a<lo];nxt=[a for a,b in known if a>=hi]
    lo2=max(b for a,b in known if b<=lo) if prev else lo; hi2=min(nxt) if nxt else hi
    sl=[(x,slot(x)) for x in sorted(tg) if lo2<=x<hi2 and not covered(x)]
    un=[(e['size'],e['name']) for e in E if e['obj']==obj and not e['hits']]
    res=[];used=set()
    for s,nm in sorted(un,reverse=True):
        c=[(abs(((s+15)&~15)-z),x,z) for x,z in sl if x not in used and z>=s*0.85 and z<=((s+15)&~15)*1.15+16]
        if c:
            c.sort();used.add(c[0][1]);res.append((c[0][1],c[0][2],s,nm))
    return sorted(res)
if len(sys.argv)>1:
    for x,z,s,nm in place(sys.argv[1]): print('%x slot %d ours %d %s'%(x,z,s,nm[:70]))
else:
    tot=collections.Counter()
    for o in sorted(set(e['obj'] for e in E)):
        r=place(o)
        if r: print(len(r),sum(s for x,z,s,n in r),o)
