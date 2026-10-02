import json,re,struct,sys,pefile,bisect
R=pefile.PE('../../cod2engineold/binaries/windows/1.0/CoD2MP_s.exe'); O=pefile.PE('build/CoD2MP_s.exe')
rd=R.get_data(0x1000,1408683)
fa={}; data=[]
for l in open('build/CoD2MP_s.map',errors='replace'):
    m=re.match(r'\s*000(\d):\S+\s+(\S+)\s+([0-9a-f]{8})( f)?',l)
    if m:
        a=int(m.group(3),16)
        if m.group(4): fa[m.group(2)]=a
        elif m.group(1)!='1': data.append((a,m.group(2)))
data.sort(); da=[d[0] for d in data]
cmp=json.load(open('build/cmp.json'))
for x in sys.argv[1:]:
    X=int(x,16); pat=struct.pack('<I',X); found={}
    for e in cmp:
        if e['name'] not in fa: continue
        for h in e['hits']:
            h=int(h,16); off=h-0x401000; seg=rd[off:off+e['size']]
            i=seg.find(pat)
            while i!=-1:
                ov=struct.unpack('<I',O.get_data(fa[e['name']]-0x400000+i,4))[0]
                k=bisect.bisect_right(da,ov)-1
                if k>=0: found[(data[k][1],ov-data[k][0])]=found.get((data[k][1],ov-data[k][0]),0)+1
                i=seg.find(pat,i+1)
    print(x, sorted(found.items(),key=lambda t:-t[1])[:3])
