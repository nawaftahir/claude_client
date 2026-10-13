"""Check the actual assignment branch, without changing its callee ABI."""
import json
from pathlib import Path
import re
import subprocess
import tempfile
import capstone
import pefile

root = Path(__file__).resolve().parents[2]
source = (root / 'target/src/script/scr_compiler.cpp').read_text()
bodies = []
for name in ('IsUndefinedPrimitiveExpression', 'IsUndefinedExpression', 'EmitAssignmentStatement'):
    bodies.append(re.search(r'^(?:bool|int|void) ' + name +
                            r'\([^;\n]+\)\n\{[\s\S]*?\n\}', source, re.M).group())
program = r'''
#include <cassert>
#include <vector>
union sval_u { int type; sval_u *node; };
struct scr_block_s {};
enum { ENUM_primitive_expression = 6, ENUM_undefined = 31 };
static bool clearSucceeded;
static std::vector<int> calls;
bool EmitClearVariableExpression(sval_u, sval_u, scr_block_s *) {
    calls.push_back(1); return clearSucceeded;
}
void EmitExpression(sval_u, scr_block_s *) { calls.push_back(2); }
void EmitVariableExpressionRef(sval_u, scr_block_s *) { calls.push_back(3); }
void EmitSetVariableField(sval_u) { calls.push_back(4); }
'''
program += '\n'.join(bodies) + r'''
int main() {
    sval_u expr[2] = {}, primitive[1] = {}, rhs = {}, unused = {};
    rhs.node = expr; expr[1].node = primitive;
    for (int outer = 0; outer < 64; ++outer) {
        for (int inner = 0; inner < 64; ++inner) {
            expr[0].type = outer; primitive[0].type = inner;
            bool undefined = outer == 6 && inner == 31;
            assert(IsUndefinedExpression(rhs) == int(undefined));
            for (int clear = 0; clear < 2; ++clear) {
                clearSucceeded = clear != 0; calls.clear();
                EmitAssignmentStatement(unused, rhs, unused, unused, 0);
                std::vector<int> expected;
                if (undefined) expected.push_back(1);
                if (!undefined || !clearSucceeded) {
                    expected.push_back(2); expected.push_back(3); expected.push_back(4);
                }
                assert(calls == expected);
            }
        }
    }
}
'''
with tempfile.TemporaryDirectory(prefix='cod2_assignment_') as directory:
    binary = str(Path(directory) / 'test')
    subprocess.run(['g++', '-std=c++11', '-O2', '-x', 'c++', '-', '-o', binary],
                   input=program, text=True, check=True)
    subprocess.run([binary], check=True)
rows = json.loads((root / 'target/build/exp.json').read_text())
row = next(row for row in rows if row['name'].startswith('?EmitAssignmentStatement@@'))
assert row['size'] == 112 and row['hits'] == ['0x46d3f0'], row
symbols = {}
for line in (root / 'target/build/CoD2MP_s.map').read_text().splitlines():
    match = re.match(r'\s*0001:\S+\s+(\S+)\s+([0-9a-f]{8}) f', line)
    if match:
        symbols.setdefault(int(match[2], 16), []).append(match[1])
ours = pefile.PE(str(root / 'target/build/CoD2MP_s.exe'))
retail = pefile.PE(str(root.parent / 'cod2engineold/binaries/windows/1.0/CoD2MP_s.exe'))
address = next(address for address, names in symbols.items() if row['name'] in names)
decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
decoder.detail = True
def calls_in(image, start):
    return {ins.address - start: ins.operands[0].imm
            for ins in decoder.disasm(image.get_data(start - image.OPTIONAL_HEADER.ImageBase, 112), start)
            if ins.mnemonic == 'call' and ins.operands[0].type == capstone.x86.X86_OP_IMM}
actual, expected = calls_in(ours, address), calls_in(retail, 0x46d3f0)
entries = {entry['name']: entry for entry in rows}
assert actual.keys() == expected.keys() and len(actual) == 5
for offset, destination in actual.items():
    assert any(hex(expected[offset]) in entries.get(name, {}).get('hits', [])
               for name in symbols.get(destination, [])), (offset, destination, expected[offset])
print('PASS: actual helpers, 8192 AST/clear fixtures, exact 112-byte match and all five call bindings')
