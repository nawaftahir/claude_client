# objorder.py obj: retail-order base names for obj (unique hits) with the source file defining each; run from target/
import json,sys,re,subprocess
es=[e for e in json.load(open('build/exp.json')) if e['obj']==sys.argv[1] and len(e['hits'])==1]
for e in sorted(es,key=lambda e:int(e['hits'][0],16)):
    m=re.match(r'\?([^@]+)@(?:([^@]+)@)?@',e['name']); n=(m.group(2)+'::'+m.group(1)) if m and m.group(2) and not m.group(1).startswith('?') else (m.group(1) if m else e['name'])
    r=subprocess.run(['grep','-rlE','^[A-Za-z].*\\b%s\\s*\\('%re.escape(n),'src','--include=*.cpp','--include=*.c'],capture_output=True,text=True).stdout.split()
    print(e['hits'][0],e['size'],n,' '.join(r))
