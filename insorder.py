# insorder.py obj src/path After=Fn1,Fn2 ...: retail order (tuorder) with unmatched fns inserted after a matched one, then reorder -a + fixuse; run from target/
import subprocess,sys
obj,f=sys.argv[1:3];m={}
for a in sys.argv[3:]:
    k,v=a.split('=');m[k]=v.split(',')
ins=set(x for v in m.values() for x in v)
o=[n for n in subprocess.run(['python3','../tuorder.py',obj,f],capture_output=True,text=True).stdout.split() if n not in ins]
r=[]
for n in o: r+=[n]+m.get(n,[])
assert all(x in r for x in ins),'anchor missing'
subprocess.run(['python3','../reorder.py','-a',f]+r,check=True)
subprocess.run(['python3','../fixuse.py',f])
