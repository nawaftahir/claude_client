# sandwich.py: unique-hit fns whose retail neighbours both belong to one other obj
import json
rows=sorted((int(e['hits'][0],16),e['name'],e['obj'],e['size']) for e in json.load(open('build/exp.json')) if len(e['hits'])==1 and e['size']>=24)
for i in range(1,len(rows)-1):
    a,n,o,s=rows[i]; p=rows[i-1][2]; q=rows[i+1][2]
    if p==q and o!=p:
        print('%x %-55s %-26s inside %s'%(a,n[:55],o,p))
