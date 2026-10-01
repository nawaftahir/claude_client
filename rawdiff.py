import pefile, re, sys, capstone
# rawdiff.py <mangled-prefix> 0xCADDR : insns whose raw bytes differ (incl. jump targets), run from target/
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
def sec(p):
    pe = pefile.PE(p); t = next(s for s in pe.sections if s.Name.startswith(b'.text'))
    return t.get_data(), pe.OPTIONAL_HEADER.ImageBase + t.VirtualAddress
o, ob = sec('build/CoD2MP_s.exe'); c, cb = sec('../../cod2engineold/binaries/windows/1.0/CoD2MP_s.exe')
syms = []
for l in open('build/CoD2MP_s.map', errors='replace'):
    r = re.match(r'\s*0001:[0-9a-f]+\s+(\S+)\s+([0-9a-f]{8}) f', l)
    if r: syms.append((int(r.group(2), 16), r.group(1)))
syms.sort()
i = next(k for k, s in enumerate(syms) if s[1].startswith(sys.argv[1]))
a, n = syms[i]; sz = len(o[a - ob:syms[i + 1][0] - ob].rstrip(b'\xcc'))
ca = int(sys.argv[2], 16)
X = list(md.disasm(o[a - ob:a - ob + sz], 0)); Y = list(md.disasm(c[ca - cb:ca - cb + sz], 0))
for x, y in zip(X, Y):
    if x.mnemonic != y.mnemonic or (x.mnemonic[0] == 'j' and x.op_str != y.op_str):
        print(hex(x.address), x.mnemonic, x.op_str, '|', y.mnemonic, y.op_str)
