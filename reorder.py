# reorder.py file name1 name2 ...: move top-level function definitions into the given order,
# placed where the first of them (in file order) sits. A function's chunk takes its leading // or /* */ lines.
import re,sys
p=sys.argv[1]; names=sys.argv[2:]
L=open(p).read().split('\n')
def chunk(n):
    if re.fullmatch(r'L\d+',n): hit=[int(n[1:])-1]  # Lnnn = def at line nnn (overloads)
    else: hit=[i for i,l in enumerate(L) if l and not l[0].isspace() and not l.startswith(('//','#','}','{'))
         and re.search(r'\b%s\s*\('%re.escape(n),l) and not l.rstrip().endswith(';')]
    # a def may wrap: "static void attr\n    Name(" ; check the previous line
    if not hit:
        hit=[i-1 for i,l in enumerate(L) if re.match(r'\s+%s\s*\('%re.escape(n),l) and i and not L[i-1].rstrip().endswith(';')]
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
hoist=[i for i in range(at,end) if decl.match(L[i])]
hl=[L[i] for i in hoist]
for i in reversed(hoist): del L[i]
L[at:at]=hl+([''] if hl else [])
at+=len(hl)+bool(hl)
L[at:at]=[x for n in names for x in body[n]]
open(p,'w').write('\n'.join(L))
