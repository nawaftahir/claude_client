import sys,subprocess,re
O='../../cod2engineold/binaries/windows/1.0/CoD2MP_s.exe'
ours={}
for l in open('build/CoD2MP_s.map',errors='replace'):
    m=re.match(r'\s*0001:\S+\s+(\S+)\s+([0-9a-f]{8}) f',l)
    if m: ours[m.group(1)]=int(m.group(2),16)
def head(exe,a):
    out=subprocess.run(['objdump','-d','--no-show-raw-insn','-M','intel',exe,'--start-address=%#x'%a,'--stop-address=%#x'%(a+12)],capture_output=True,text=True).stdout
    return 'and    esp' in out
for l in open(sys.argv[1]):
    d,s,a,m=l.split()[:4]
    if m not in ours: continue
    o=head('build/CoD2MP_s.exe',ours[m]); r=head(O,int(a,16))
    if o!=r: print(d,s,a,m,'ours' if o else 'retail')
