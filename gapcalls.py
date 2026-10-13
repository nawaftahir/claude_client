# gapcalls.py obj [min]: retail E8 call targets that fall inside obj's gaps = function starts there; run from target/
import json,struct,sys,subprocess
d=open('../../cod2engineold/binaries/windows/1.0/CoD2MP_s.exe','rb').read()
pe=struct.unpack_from('<I',d,0x3c)[0];ns=struct.unpack_from('<H',d,pe+6)[0];oh=struct.unpack_from('<H',d,pe+20)[0]
sec=pe+24+oh
for i in range(ns):
    n,vs,va,rs,ro=struct.unpack_from('<8sIIII',d,sec+40*i)
    if n.startswith(b'.text'): tva,to,tsz=0x400000+va,ro,vs
t=d[to:to+tsz]
tg={}
for i in range(len(t)-5):
    if t[i]==0xe8:
        x=tva+i+5+struct.unpack_from('<i',t,i+1)[0]
        if tva<=x<tva+tsz: tg[x]=tg.get(x,0)+1
es=sorted(((int(e['hits'][0],16),e['size'],e['name']) for e in json.load(open('build/exp.json')) if e['obj']==sys.argv[1] and len(e['hits'])==1))
mn=int(sys.argv[2]) if len(sys.argv)>2 else 16
for (a,s,n),(b,_,m) in zip(es,es[1:]):
    if b-((a+s+15)&~15)>=mn and b-a<0x10000:
        st=sorted(x for x in tg if a+s<=x<b and x%16==0)
        print('%x..%x after %s: %s'%(a+s,b,n[:36],' '.join('%x(%d)'%(x,tg[x]) for x in st)))
