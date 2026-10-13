"""Prove the lightweight decoder preserves every instruction and call mask."""
from pathlib import Path
import re
import time

import capstone
import pefile

root = Path(__file__).resolve().parents[2]
pe = pefile.PE(str(root / 'target/build/CoD2MP_s.exe'))
section = next(s for s in pe.sections if s.Name.startswith(b'.text'))
data = section.get_data()
base = pe.OPTIONAL_HEADER.ImageBase + section.VirtualAddress
symbols = sorted(int(m[1], 16) for line in
                 (root / 'target/build/CoD2MP_s.map').read_text().splitlines()
                 if (m := re.match(r'\s*0001:\S+\s+\S+\s+([0-9a-f]{8}) f', line))
                 and base <= int(m[1], 16) < base + len(data))
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
objects_seconds = lite_seconds = 0
count = 0
for index, address in enumerate(symbols):
    end = symbols[index+1] if index+1 < len(symbols) else base+len(data)
    code = data[address-base:end-base].rstrip(b'\xcc')
    if len(code) < 6:
        continue
    begin = time.perf_counter()
    objects = list(md.disasm(code, address))
    objects_seconds += time.perf_counter() - begin
    begin = time.perf_counter()
    lite = list(md.disasm_lite(code, address))
    lite_seconds += time.perf_counter() - begin
    assert [(i.address, i.size, i.mnemonic, i.op_str) for i in objects] == lite
    old, new = set(), set()
    for ins in objects:
        if ins.mnemonic in ('call', 'jmp') and ins.size == 5 and ins.bytes[0] in (0xe8, 0xe9):
            target = int.from_bytes(ins.bytes[1:], 'little', signed=True) + ins.address + 5
            if not address <= target < address+len(code):
                old.update(range(ins.address-address+1, ins.address-address+5))
    for here, size, mnemonic, _ in lite:
        offset = here-address
        if mnemonic in ('call', 'jmp') and size == 5 and code[offset] in (0xe8, 0xe9):
            target = int.from_bytes(code[offset+1:offset+5], 'little', signed=True) + here + 5
            if not address <= target < address+len(code):
                new.update(range(offset+1, offset+5))
    assert old == new, hex(address)
    count += 1
assert count > 7000
print(f'PASS: {count} complete instruction streams/masks identical; '
      f'decode {objects_seconds:.3f}s objects / {lite_seconds:.3f}s lightweight')
