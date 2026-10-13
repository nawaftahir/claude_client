# reorder.py [-n] file name1 name2 ...: (-n: don't hoist decls; -a: hoist all non-function lines) move top-level function definitions into the given order,
# placed where the first of them (in file order) sits. A function's chunk takes its leading // or /* */ lines.
import re,sys
nohoist='-n' in sys.argv; args=[a for a in sys.argv[1:] if a not in('-n','-a')]
p=args[0]; names=args[1:]
L=open(p).read().split('\n')
inc=set();c=False  # lines inside /* */ blocks
for i,l in enumerate(L):
    if c or l.lstrip().startswith('/*'): inc.add(i)
    if l.lstrip().startswith('/*'): c=True
    if '*/' in l: c=False
def proto(i):  # statement ends with ';' before any '{'
    for k in range(i,min(i+8,len(L))):
        if '{' in L[k]: return False
        if L[k].rstrip().endswith(';'): return True
    return False
def chunk(n):
    if re.fullmatch(r'L\d+',n): hit=[int(n[1:])-1]  # Lnnn = def at line nnn (overloads)
    else: hit=[i for i,l in enumerate(L) if l and not l[0].isspace() and not l.startswith(('//','#','}','{'))
         and re.search(r'\b%s\s*\('%re.escape(n),l) and i not in inc and not proto(i)]
    # a def may wrap: "static void attr\n    Name(" ; check the previous line
    if not hit:
        hit=[i-1 for i,l in enumerate(L) if re.match(r'\s+(?:[\w\*]+\s+)*%s\s*\('%re.escape(n),l) and i and L[i-1][:1].isalpha() and not L[i-1].rstrip().endswith((';','{','}'))]
    assert len(hit)==1,(n,hit)
    s=hit[0]
    while s and L[s-1].startswith('//'): s-=1
    if s and L[s-1].rstrip()=='*/':
        while not L[s-1].startswith('/*'): s-=1
        s-=1
    if '{' in L[hit[0]] and L[hit[0]].rstrip().endswith('}'): return s,hit[0]+1  # one-line function
    e=next(i for i in range(hit[0],len(L)) if L[i]=='}')+1
    if e<len(L) and L[e]=='': e+=1
    return s,e
cs=sorted((chunk(n)+(n,) for n in names))
for a,b in zip(cs,cs[1:]): assert a[1]<=b[0],(a,b)
body={n:L[s:e] for s,e,n in cs}
at=cs[0][0]
for s,e,n in reversed(cs): del L[s:e]
# hoist single-line externs, prototypes and #defines left between the moved functions to the insertion point
end=cs[-1][1]-sum(e-s for s,e,n in cs)
decl=re.compile(r'(extern\s.*;|[A-Za-z][^=]*\)\s*;|#define\s[^\\]*)\s*$')
hoist=[] if nohoist else [i for i in range(at,end) if decl.match(L[i])]
if '-a' in sys.argv:  # hoist every top-level line that is not part of a leftover function
    hoist=[];i=at;pend=[]
    while i<end:
        l=L[i]
        if l.startswith('//') or (l.startswith('/*') and l.rstrip().endswith('*/')): pend.append(i);i+=1;continue
        isdef=bool(re.match(r'[A-Za-z_].*\(',l)) and not l.rstrip().endswith(';') and not l.startswith(('#','typedef','struct','enum','extern'))
        if isdef:
            j=next(k for k in range(i,end) if L[k]=='}' or (k==i and '{' in l and l.rstrip().endswith('}')))
            pend=[];i=j+1;continue
        hoist+=pend+[i];pend=[];i+=1
    hoist+=pend
hl=[L[i] for i in hoist]
for i in reversed(hoist): del L[i]
L[at:at]=hl+([''] if hl else [])
at+=len(hl)+bool(hl)
L[at:at]=[x for n in names for x in body[n]]
open(p,'w').write('\n'.join(L))
