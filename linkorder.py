# linkorder.py: objs whose retail code position disagrees with our link order
import json,statistics
order=[l.strip().split('/')[-1].rsplit('.',1)[0]+'.obj' for l in open('src/tus.txt') if l.strip() and not l.startswith('#')]
pos={o:i for i,o in enumerate(order)}
by={}
for e in json.load(open('build/exp.json')):
    if len(e['hits'])==1: by.setdefault(e['obj'],[]).append(int(e['hits'][0],16))
med={o:statistics.median(v) for o,v in by.items() if len(v)>=2 and o in pos}
seq=sorted(med,key=lambda o:med[o])
# longest increasing subsequence of our positions along retail order
p=[pos[o] for o in seq]
import bisect
tails=[];idx=[];prev=[-1]*len(p)
for i,x in enumerate(p):
    k=bisect.bisect_left([p[j] for j in idx],x)
    if k==len(idx): idx.append(i)
    else: idx[k]=i
    prev[i]=idx[k-1] if k else -1
keep=set(); i=idx[-1]
while i!=-1: keep.add(i); i=prev[i]
for i,o in enumerate(seq):
    if i not in keep:
        before=seq[i-1] if i else None; after=seq[i+1] if i+1<len(seq) else None
        print('%-28s retail %x  ours#%d  (retail between %s and %s)'%(o,int(med[o]),pos[o],before,after))
