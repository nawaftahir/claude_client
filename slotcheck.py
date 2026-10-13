# slotcheck.py [obj]: per run of unmatched fns between two matched neighbours (our order), compare our size sum to the retail gap; run from target/
import json,sys
m={}
for l in open('build/CoD2MP_s.map'):
    p=l.split()
    if len(p)>=3 and p[0].startswith('0001:'):
        try: m[p[1]]=int(p[2],16)
        except ValueError: pass
E=json.load(open('build/exp.json'))
al=lambda s:(s+15)&~15
for obj in sorted(set(e['obj'] for e in E)) if len(sys.argv)<2 else sys.argv[1:]:
    fs=sorted(((m[e['name']],e) for e in E if e['obj']==obj and e['name'] in m),key=lambda x:x[0])
    rh=sorted(int(e['hits'][0],16) for a,e in fs if len(e['hits'])==1)
    if not rh: continue
    rmed=rh[len(rh)//2]
    if not fs: continue
    med=sorted(a for a,e in fs)[len(fs)//2];fs=[(a,e) for a,e in fs if abs(a-med)<0x40000]
    prev=None;run=[]
    for a,e in fs:
        if len(e['hits'])==1 and abs(int(e['hits'][0],16)-rmed)<0x40000:
            r=int(e['hits'][0],16)
            if run and prev:
                gap=r-al(prev[0]+prev[1]);need=sum(al(x['size']) for x in run)
                if abs(gap-need)>64 or gap<0: print('%-24s gap %6d need %6d after %s: %s'%(obj,gap,need,prev[2][:30],' '.join(x['name'][:30] for x in run)))
            prev=(r,e['size'],e['name']);run=[]
        elif not e['hits']: run.append(e)
