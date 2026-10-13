# gaps.py obj [min]: gaps between consecutive retail functions of obj (unique hits); run from target/
import json,sys
es=sorted(((int(e['hits'][0],16),e['size'],e['name']) for e in json.load(open('build/exp.json')) if e['obj']==sys.argv[1] and len(e['hits'])==1))
mn=int(sys.argv[2]) if len(sys.argv)>2 else 16
for (a,s,n),(b,_,m) in zip(es,es[1:]):
    g=b-((a+s+15)&~15)
    if g>=mn: print('%x..%x gap %d after %s before %s'%(a+s,b,g,n[:40],m[:40]))
for e in json.load(open('build/exp.json')):
    if e['obj']==sys.argv[1] and not e['hits']: print('unplaced',e['size'],e['name'][:60])
