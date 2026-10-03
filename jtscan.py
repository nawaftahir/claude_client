import re,sys,struct,bisect
SP=sys.argv[1]  # usage (in target/): jtscan.py $SP [mangled 0xADDR size]; needs $SP/retail.asm, ours.asm, ad30.txt
def load(p):
    d=open(p,'rb').read()
    pe=struct.unpack_from('<I',d,0x3c)[0]; ns=struct.unpack_from('<H',d,pe+6)[0]; oh=struct.unpack_from('<H',d,pe+20)[0]
    secs=[]
    for i in range(ns):
        o=pe+24+oh+40*i; vs,va,rs,ro=struct.unpack_from('<IIII',d,o+8); secs.append((va+0x400000,rs,ro))
    def rd(v,sz=4):
        for va,rs,ro in secs:
            if va<=v<va+rs: return struct.unpack_from('<I' if sz==4 else '<B',d,ro+v-va)[0]
    return rd
rr=load('../../cod2engineold/binaries/windows/1.0/CoD2MP_s.exe'); ro=load('build/CoD2MP_s.exe')
def asm(fn):
    a=[];t=[]
    for l in open(fn):
        m=re.match(r'\s+([0-9a-f]+):\s+(.*)',l)
        if m: a.append(int(m.group(1),16)); t.append(m.group(2))
    return a,t
RA,RT=asm(SP+'/retail.asm'); OA,OT=asm(SP+'/ours.asm')
mp={}
for l in open('build/CoD2MP_s.map'):
    p=l.split()
    if len(p)>=4 and p[0].startswith('0001:'):
        try: mp[p[1]]=int(p[2],16)
        except: pass
def tables(A,T,rd,start,size):
    i=bisect.bisect_left(A,start); res=[]
    while i<len(A) and A[i]<start+size:
        m=re.match(r'jmp\s+DWORD PTR \[(\w+)\*4\+0x([0-9a-f]+)\]',T[i])
        if m:
            base=int(m.group(2),16); bt=None
            mz=re.match(r'movzx\s+\w+,BYTE PTR \[(\w+)\+0x([0-9a-f]+)\]',T[i-1])
            if mz: bt=int(mz.group(2),16)
            n=None
            for k in range(i-1,max(i-8,0),-1):
                c=re.match(r'cmp\s+\w+,0x([0-9a-f]+)$',T[k])
                if c: n=int(c.group(1),16)+1; break
            if n and n<600:
                if bt: tg=[rd(base+4*rd(bt+j,1)) for j in range(n)]
                else: tg=[rd(base+4*j) for j in range(n)]
                order=sorted(set(tg)); res.append([tuple(j for j in range(n) if tg[j]==a) for a in order])
        i+=1
    return res
if len(sys.argv)>2:
    name=sys.argv[2]; ca=int(sys.argv[3],16); sz=int(sys.argv[4])
    for x in tables(RA,RT,rr,ca,sz): print('retail',x)
    for x in tables(OA,OT,ro,mp[name],sz+100): print('ours  ',x)
    sys.exit()
for l in open(SP+'/ad30.txt'):
    p=l.split()
    if len(p)<4 or p[0]=='0': continue
    ca=int(p[2],16); oa=mp.get(p[3]); sz=int(p[1])
    if oa is None: continue
    r=tables(RA,RT,rr,ca,sz); o=tables(OA,OT,ro,oa,sz)
    if len(r)!=len(o): continue
    for x,y in zip(r,o):
        if x!=y:
            print('ORDER' if sorted(x)==sorted(y) else 'GROUP',p[0],sz,p[2],p[3]); break
