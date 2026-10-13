# fixuse.py src/path: after a reorder, move each function reported "identifier not found" to just before its first user; loops via tu.sh
import re,subprocess,sys
p=sys.argv[1]
for it in range(20):
    out=subprocess.run(['../tu.sh',p],capture_output=True,text=True).stdout
    errs=re.findall(r'\((\d+)\) : error C3861: \'(\w+)\'',out)
    if not errs: print(out.strip()); break
    ln,n=int(errs[0][0]),errs[0][1]
    L=open(p).read().split('\n')
    u=[l for l in L[:ln] if re.match(r'[A-Za-z_].*\(',l) and not l.rstrip().endswith(';')][-1]
    un=re.findall(r'(\w+)\s*\(',u)[0] if '(' in u else None
    un=[w for w in re.findall(r'([\w:~]+)\s*\(',u)][0]
    print('move',n,'before',un)
    r=subprocess.run(['python3','../reorder.py','-n',p,n,un],capture_output=True,text=True)
    if r.returncode: print(r.stderr.strip().split('\n')[-1]); break
