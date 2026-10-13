# dwdiff.py RVA OVA N: dword diff retail vs ours, ours relocs shown as R
import sys,struct,pefile
B=0x400000
r=pefile.PE('../../cod2engineold/binaries/windows/1.0/CoD2MP_s.exe').get_memory_mapped_image()
op=pefile.PE('build/CoD2MP_s.exe'); o=op.get_memory_mapped_image()
rel=set(B+e.rva for b in op.DIRECTORY_ENTRY_BASERELOC for e in b.entries if e.type==3)
ra,oa,n=(int(x,16) for x in sys.argv[1:4])
for k in range(0,n,4):
    a=struct.unpack_from('<I',r,ra-B+k)[0]; b=struct.unpack_from('<I',o,oa-B+k)[0]
    m='R' if oa+k in rel else ' '
    if a!=b and m!='R' or len(sys.argv)>4: print('%x %x %08x %08x %s%s'%(ra+k,oa+k,a,b,m,'' if a==b else ' *'))
