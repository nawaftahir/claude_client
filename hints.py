# hints.py <retail exe> <our exe>: compare IAT import hints (they pin the import libs / SDK version)
import struct,sys
def imports(p):
    d=open(p,'rb').read(); pe=struct.unpack_from('<I',d,0x3c)[0]
    ns=struct.unpack_from('<H',d,pe+6)[0]; osz=struct.unpack_from('<H',d,pe+20)[0]
    opt=pe+24; secs=[struct.unpack_from('<8sIIII',d,opt+osz+i*40) for i in range(ns)]
    def off(rva):
        for n,vs,va,rs,ro in secs:
            if va<=rva<va+max(vs,rs): return rva-va+ro
    irva=struct.unpack_from('<I',d,opt+96+8)[0]; o=off(irva); r={}
    while True:
        oft,_,_,name,ft=struct.unpack_from('<IIIII',d,o); o+=20
        if not name: break
        dn=d[off(name):].split(b'\0')[0].decode().lower(); t=off(oft or ft)
        while True:
            e=struct.unpack_from('<I',d,t)[0]; t+=4
            if not e: break
            if e&0x80000000: r[(dn,'#%d'%(e&0xffff))]=None; continue
            h=struct.unpack_from('<H',d,off(e))[0]; fn=d[off(e)+2:].split(b'\0')[0].decode(); r[(dn,fn)]=h
    return r
a=imports(sys.argv[1]); b=imports(sys.argv[2])
same=sum(1 for k in a if k in b and a[k]==b[k]); diff=[(k,a[k],b[k]) for k in a if k in b and a[k]!=b[k]]
print('common',sum(1 for k in a if k in b),'same hint',same,'diff',len(diff)); [print(x) for x in diff[:8]]
from collections import Counter
c=Counter((k[0],a[k]==b[k]) for k in a if k in b); print(sorted(c.items()))
