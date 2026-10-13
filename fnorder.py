# fnorder.py [obj]: within-obj function order, retail (unique hits) vs ours (map); run from target/
import json,sys,bisect
m={}
for l in open('build/CoD2MP_s.map'):
    p=l.split()
    if len(p)>=3 and p[0].startswith('0001:'):
        try: m[p[1]]=int(p[2],16)
        except ValueError: pass
by={}
for e in json.load(open('build/exp.json')):
    if len(e['hits'])==1 and e['name'] in m and e['size']>=16:
        by.setdefault(e['obj'],[]).append((int(e['hits'][0],16),m[e['name']],e['name']))
res=[]
for o,v in by.items():
    v.sort(); ours=[b for a,b,n in v]
    # functions outside the longest increasing run of our addresses = misplaced
    idx=[];prev=[-1]*len(ours)
    for i,x in enumerate(ours):
        k=bisect.bisect_left([ours[j] for j in idx],x)
        if k==len(idx): idx.append(i)
        else: idx[k]=i
        prev[i]=idx[k-1] if k else -1
    keep=set(); i=idx[-1] if idx else -1
    while i!=-1: keep.add(i); i=prev[i]
    bad=[v[i] for i in range(len(v)) if i not in keep]
    if bad: res.append((len(bad),o,bad))
if len(sys.argv)>1:
    for c,o,bad in res:
        if o==sys.argv[1]:
            for a,b,n in by[o]: print('%x %x %s%s'%(a,b,n[:60],'  <-- moved' if (a,b,n) in bad else ''))
else:
    for c,o,bad in sorted(res,reverse=True): print(c,o,len(by[o]))
