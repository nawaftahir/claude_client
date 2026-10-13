# tuorder.py obj src/path: retail-order names of obj's functions defined in that file (unique hits, main cluster); run from target/
import json,sys,re,subprocess
obj,path=sys.argv[1],sys.argv[2]
src=open(path).read()
es=sorted((e for e in json.load(open('build/exp.json')) if e['obj']==obj and len(e['hits'])==1),key=lambda e:int(e['hits'][0],16))
a=[int(e['hits'][0],16) for e in es]; med=a[len(a)//2] if a else 0
out=[]
for e in es:
    if abs(int(e['hits'][0],16)-med)>0x40000: continue  # inline copies emitted elsewhere
    m=re.match(r'\?([^@]+)@(?:([^@]+)@)?@',e['name'])
    if not m or m.group(1).startswith('?'): continue
    n=(m.group(2)+'::'+m.group(1)) if m.group(2) else m.group(1)
    if re.search(r'^[A-Za-z][^;\n]*\b%s\s*\([^;]*$'%re.escape(n),src,re.M) and n not in out: out.append(n)
print(' '.join(out))
