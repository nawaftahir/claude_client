import sys,re,pefile,capstone
O='../../cod2engineold/binaries/windows/1.0/CoD2MP_s.exe'
def load(p):
    pe=pefile.PE(p); s=[x for x in pe.sections if x.Name.startswith(b'.text')][0]
    return s.get_data(), pe.OPTIONAL_HEADER.ImageBase+s.VirtualAddress
od,ob=load('build/CoD2MP_s.exe'); rd,rb=load(O)
ours={}
for l in open('build/CoD2MP_s.map',errors='replace'):
    m=re.match(r'\s*0001:\S+\s+(\S+)\s+([0-9a-f]{8}) f',l)
    if m: ours[m.group(1)]=int(m.group(2),16)
md=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32)
def frame(d,b,a):
    for i,ins in enumerate(md.disasm(d[a-b:a-b+40],a)):
        if i>6: break
        if ins.mnemonic=='sub' and ins.op_str.startswith('esp'): return ins.op_str
        if ins.mnemonic=='mov' and ins.op_str.startswith('eax, 0x') : return 'chk '+ins.op_str
    return None
for l in open(sys.argv[1]):
    p=l.split()
    if len(p)<4 or p[0]=='0': continue
    d,s,a,m=p[:4]
    if m not in ours: continue
    fo=frame(od,ob,ours[m]); fr=frame(rd,rb,int(a,16))
    if fo!=fr: print(d,s,a,m,fo,'|',fr)
