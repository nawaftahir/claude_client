#!/usr/bin/env python3
"""Parallel experiment runner: N worktree slots, each builds + measures one experiment at a time.
  lab.py init [N=4]          create slots ../lab/s1..sN at HEAD, seeded from target/build (no full rebuild)
  lab.py sync                move every slot to HEAD (after a commit on master) and rebuild its baseline
  lab.py run exps.json [out.jsonl]
exps.json: [{"id": "...", "edits": [{"file": "src/...", "old": "...", "new": "..."}]}]  (paths relative to target/;
`old` must occur exactly once; or {"file", "line": N, "old": whole line, "new": whole line}). Result per experiment: net unique bytes vs the slot baseline + gained/lost fns.
Only net bytes decide (LTCG ripple is included). Slots are reverted after every experiment."""
import json, os, queue, re, shutil, subprocess, sys, threading, time
D = os.path.dirname(os.path.abspath(__file__))
LAB = os.path.abspath(D + '/../lab')
ANCHOR = os.path.abspath(D + '/../cod2engineold/binaries/windows/1.0/CoD2MP_s.exe')

def sh(cmd, cwd, **kw):
    return subprocess.run(cmd, cwd=cwd, shell=True, capture_output=True, text=True, **kw)

def slots():
    only = os.environ.get('LAB_SLOTS', '').split()  # e.g. LAB_SLOTS="s1 s2" while other slots are busy
    return sorted((p for p in (os.path.join(LAB, s) for s in os.listdir(LAB))
                   if os.path.isdir(p + '/target') and (not only or os.path.basename(p) in only)),
                  key=lambda p: int(p.rsplit('s', 1)[1])) if os.path.isdir(LAB) else []

def cov_bytes(cmpjson, cwd):
    out = sh('python3 %s/cov.py %s' % (D, cmpjson), cwd).stdout
    return int(re.search(r'unique bytes (\d+)', out).group(1))

def measure(t, out):
    """build + cmp in slot target dir t -> (ok, unique bytes, log tail)"""
    r = sh('make > build/b.txt 2>&1; echo $?', t)
    log = open(t + '/build/b.txt', errors='replace').read()
    if r.stdout.strip() != '0' or re.search(r' error C\d+| error LNK| fatal error', log):
        return False, 0, '\n'.join(l for l in log.splitlines() if 'error' in l)[:2000]
    sh('python3 %s/cmp.py build/CoD2MP_s.exe build/CoD2MP_s.map %s build/%s >/dev/null' % (D, ANCHOR, out), t)
    return True, cov_bytes('build/' + out, t), ''

def baseline(s):
    t = s + '/target'
    ok, b, err = measure(t, 'base.json')
    if not ok:
        sys.exit('baseline build failed in %s:\n%s' % (s, err))
    return b

def init(n):
    os.makedirs(LAB, exist_ok=True)
    for name in ('cod2engineold', 'refrences'):
        if not os.path.lexists(LAB + '/' + name):
            os.symlink('../' + name, LAB + '/' + name)
    head = sh('git rev-parse HEAD', D).stdout.strip()
    for k in range(1, n + 1):
        s = '%s/s%d' % (LAB, k)
        if not os.path.isdir(s):
            r = sh('git worktree add --detach %s %s' % (s, head), D)
            if r.returncode: sys.exit(r.stderr)
            shutil.copytree(D + '/target/build', s + '/target/build', symlinks=True)
            # sources older than the copied objects, so only real edits rebuild
            sh("find src -type f -exec touch -d '2020-01-01' {} +", s + '/target')
        print(s, 'baseline', baseline(s))

def sync():
    head = sh('git rev-parse HEAD', D).stdout.strip()
    for s in slots():
        r = sh('git checkout -q --detach %s' % head, s)
        if r.returncode: sys.exit(s + ': ' + r.stderr)
        print(s, head[:7], 'baseline', baseline(s))

def apply(t, edits):
    """-> list of (path, original text) or raises ValueError"""
    orig = {}
    for e in edits:
        p = os.path.join(t, e['file'])
        if p not in orig:
            orig[p] = open(p, errors='surrogateescape').read()
        cur = open(p, errors='surrogateescape').read()
        if 'line' in e:  # whole-line replacement by 1-based number, checked against `old`
            L = cur.split('\n')
            if L[e['line'] - 1] != e['old']:
                for q, o in orig.items(): open(q, 'w', errors='surrogateescape').write(o)
                raise ValueError('%s:%d does not match' % (e['file'], e['line']))
            L[e['line'] - 1] = e['new']
            open(p, 'w', errors='surrogateescape').write('\n'.join(L))
            continue
        if cur.count(e['old']) != 1:
            for q, o in orig.items(): open(q, 'w', errors='surrogateescape').write(o)
            raise ValueError('%s: old text found %d times' % (e['file'], cur.count(e['old'])))
        open(p, 'w', errors='surrogateescape').write(cur.replace(e['old'], e['new']))
    return orig

def diffhits(a, b):
    A = {x['name']: x for x in json.load(open(a))}
    B = {x['name']: x for x in json.load(open(b))}
    ok = lambda x: x is not None and len(x['hits']) == 1
    g = [(B[n]['size'], n) for n in B if ok(B[n]) and not ok(A.get(n)) and not n.startswith('$L')]
    l = [(A[n]['size'], n) for n in A if ok(A[n]) and not ok(B.get(n)) and not n.startswith('$L')]
    return sorted(g, reverse=True), sorted(l, reverse=True)

def worker(s, q, res, lock, outf):
    t = s + '/target'
    base = cov_bytes('build/base.json', t)
    while True:
        try: e = q.get_nowait()
        except queue.Empty: return
        t0 = time.time()
        r = {'id': e['id'], 'slot': os.path.basename(s)}
        try:
            orig = apply(t, e['edits'])
        except (ValueError, OSError) as x:
            r.update(ok=False, err=str(x))
        else:
            ok, b, err = measure(t, 'exp.json')
            dropped = []
            for _ in range(3):  # drop edits in files that fail to compile, retry the rest
                if ok: break
                badf = {'src/' + m.replace('\\', '/') for m in re.findall(r'^(\S+?)\(\d+\) : (?:fatal )?error', err, re.M)}
                hit = [p for p in orig if os.path.relpath(p, t) in badf]
                if not hit: break
                for p in hit:
                    open(p, 'w', errors='surrogateescape').write(orig[p])
                dropped += [x.get('fn', x['file']) for x in e['edits'] if os.path.join(t, x['file']) in hit]
                ok, b, err = measure(t, 'exp.json')
            r.update(ok=ok, err=err, dropped=dropped)
            if ok:
                g, l = diffhits(t + '/build/base.json', t + '/build/exp.json')
                r.update(net=b - base, gained=g, lost=l[:20])
            for p, o in orig.items():
                open(p, 'w', errors='surrogateescape').write(o)
        r['secs'] = int(time.time() - t0)
        with lock:
            res.append(r)
            outf.write(json.dumps(r) + '\n'); outf.flush()
            print('%-40s %s' % (r['id'][:40], ('net %+d' % r['net']) if r.get('ok') else 'FAIL ' + r['err'][:80]), flush=True)

def run(path, out):
    exps = json.load(open(path))
    q = queue.Queue()
    for e in exps: q.put(e)
    res, lock = [], threading.Lock()
    with open(out, 'a') as f:
        ts = [threading.Thread(target=worker, args=(s, q, res, lock, f)) for s in slots()]
        for th in ts: th.start()
        for th in ts: th.join()
    # leave every slot's objects matching its (reverted) sources
    for s in slots(): sh('make > build/b.txt 2>&1', s + '/target')
    win = sorted((r for r in res if r.get('ok') and r['net'] > 0), key=lambda r: -r['net'])
    print('%d experiments, %d net-positive: %s' % (len(res), len(win), ' '.join('%s(%+d)' % (r['id'], r['net']) for r in win)))

if __name__ == '__main__':
    a = sys.argv[1:]
    if a[0] == 'init': init(int(a[1]) if len(a) > 1 else 4)
    elif a[0] == 'sync': sync()
    elif a[0] == 'run': run(a[1], a[2] if len(a) > 2 else os.path.splitext(a[1])[0] + '.out.jsonl')
