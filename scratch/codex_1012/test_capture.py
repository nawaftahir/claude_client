"""Exercise the actual capture-ring arithmetic and the split generator's branch scope."""
import importlib.util
import subprocess
import tempfile
from pathlib import Path

root = Path(__file__).resolve().parents[2]
source = (root / 'target/src/groupvoice/record_dsound.cpp').read_text()
start = source.index('if ( dwReadPos < pRecSample->dwCaptureOffset )')
end = source.index('if ( !lLockSize )', start)
body = source[start:end]
baseline = subprocess.run(['git', 'show', 'e30122f:target/src/groupvoice/record_dsound.cpp'],
                          cwd=root, capture_output=True, text=True, check=True).stdout
start = baseline.index('if ( dwReadPos < pRecSample->dwCaptureOffset )')
end = baseline.index('if ( !lLockSize )', start)
baseline = baseline[start:end]
spec = importlib.util.spec_from_file_location('split', root / 'splitsw.py')
import sys
sys.path.insert(0, str(root))
split = importlib.util.module_from_spec(spec)
spec.loader.exec_module(split)
variant = split.cands(['  x = a + b;'], 1, 1)[0][2][0]
code = '#include <assert.h>\nstruct Sample{unsigned dwCaptureOffset,dwBufferSize;};\n'
code += 'int actual(unsigned dwReadPos,Sample*pRecSample){int lLockSize;\n'
code += body + '\nreturn lLockSize;}\n'
code += 'int original(unsigned dwReadPos,Sample*pRecSample){int lLockSize;\n' + baseline
code += '\nreturn lLockSize;}\nint main(){\nSample wrapped={20,64}; assert(original(0,&wrapped)==24); assert(actual(0,&wrapped)==44);\n'
code += '''for(unsigned size=4;size<=128;size+=4)
for(unsigned offset=0;offset<size;offset++)
for(unsigned read=0;read<size;read++){
Sample sample={offset,size}; unsigned expected=(read+size-offset)%size;
expected-=expected%4; assert(actual(read,&sample)==(int)expected);}
int x=9,a=2,b=3;
if(false)
'''
code += variant + '\nassert(x==9);\nif(true)\n' + variant + '\nassert(x==5);\n}\n'
with tempfile.TemporaryDirectory(prefix='codex_capture_') as tmp:
    source = Path(tmp) / 'test.cpp'
    binary = Path(tmp) / 'test'
    source.write_text(code)
    subprocess.run(['g++', '-O2', str(source), '-o', str(binary)], check=True)
    subprocess.run([str(binary)], check=True)
print('PASS: capture wrap/non-wrap/alignment and split-generator branch scope')
