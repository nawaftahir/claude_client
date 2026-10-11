"""Execute the actual enemy-ping fade branch, including a future timestamp."""
import subprocess
import tempfile
from pathlib import Path

root = Path(__file__).resolve().parents[2]
source = (root / 'target/src/oc/cgame_mp/cg_compassfriendlies_mp.cpp').read_text()
start = source.rindex('if (cg->compassActors[i].beginFadeTime >= cg->time)')
end = source.index('UI_DrawHandlePic(x, y, w, h', start)
body = source[start:end]
code = '#include <assert.h>\n'
code += '''struct Actor{int beginFadeTime;};struct CG{int time;Actor compassActors[1];};
struct Dvar{struct{float value;}current;};
float actual(int now,int begin,float cap){
CG state={now,{{begin}}}; CG*cg=&state;
Dvar dvar={{2.0f}}; Dvar*cg_hudCompassSoundPingFadeTime=&dvar;
int i=0;float fadedColor[4]={0,0,0,0};float compassFadeOutAlpha=cap;
'''
code += body + '\nreturn fadedColor[3];}\n'
code += '''int main(){assert(actual(100,200,1)==1);assert(actual(100,100,1)==1);
assert(actual(1100,100,1)==0.5f);assert(actual(2100,100,1)==0);
assert(actual(100,200,0.25f)==0.25f);assert(actual(1100,100,0.25f)==0.25f);
assert(actual(100,200,2)==1);}
'''
with tempfile.TemporaryDirectory(prefix='codex_compass_') as tmp:
    source = Path(tmp) / 'test.cpp'
    binary = Path(tmp) / 'test'
    source.write_text(code)
    subprocess.run(['g++', '-O2', str(source), '-o', str(binary)], check=True)
    subprocess.run([str(binary)], check=True)
print('PASS: actual enemy ping future/current/fading timestamps and alpha cap')
