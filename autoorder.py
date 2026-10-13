# autoorder.py obj...: put each obj's source files in retail fn order; reverts a file that fails to compile or loses lines; run from target/
import subprocess,sys,collections
def sh(*a): return subprocess.run(a,capture_output=True,text=True)
for obj in sys.argv[1:]:
    files=collections.Counter(l.split()[-1] for l in sh('python3','../objorder.py',obj).stdout.split('\n') if l.strip().split() and l.split()[-1].startswith('src/'))
    for f,c in files.items():
        if c<2: continue
        names=sh('python3','../tuorder.py',obj,f).stdout.split()
        if len(names)<2: continue
        r=sh('python3','../reorder.py','-a',f,*names)
        ok=r.returncode==0
        if ok:
            r=sh('python3','../fixuse.py',f); out=r.stdout.strip().split('\n')
            ok=r.returncode==0 and 'error' not in r.stdout
            a=sorted(x for x in sh('git','show','HEAD:target/'+f).stdout.split('\n') if x.strip())
            b=sorted(x for x in open(f).read().split('\n') if x.strip())
            ok=ok and a==b
        if not ok:
            subprocess.run('git diff %s | git apply -R'%f,shell=True)
            print('FAIL',obj,f,(r.stderr or r.stdout).strip().split('\n')[-1][:100])
        else: print('ok',obj,f,len(names))
