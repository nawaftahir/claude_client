# symdump.py R|O START END > file : symbolic dword dump (strings, code, raw)
import sys,struct,pefile
B=0x400000
path={'R':'../../cod2engineold/binaries/windows/1.0/CoD2MP_s.exe','O':'build/CoD2MP_s.exe'}[sys.argv[1]]
r=pefile.PE(path).get_memory_mapped_image()
lo,hi=int(sys.argv[2],16),int(sys.argv[3],16)
def cs(a):
    e=r.index(b'\0',a-B); return r[a-B:e].decode('latin1')[:28]
for a in range(lo,hi,4):
    v=struct.unpack_from('<I',r,a-B)[0]
    t=repr(cs(v)) if 0x559000<=v<0x58b000 else ('C' if 0x401000<=v<0x559000 else ('P' if 0x58b000<=v<0x2000000 else '%x'%v))
    print(t)
