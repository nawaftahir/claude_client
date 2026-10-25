# c2.dll register-allocation probe (10-18, first pass)

Status (10-21): the colour choice is found (0x107833f2, see round 5) and logged by cave_rd.s + cpick.py.
Older notes below: the dump showed color.c candidate nodes when they are freed. The field meanings
below are HYPOTHESIS unless tagged PINNED.

## Run it

    T=dev/claude_client/tools_oracle; VC=dev/lab/s6/target/build/vc
    as --32 $T/cave_ra.s -o $VC/ra/cave_ra.o
    ld -m elf_i386 -Ttext=0x10865d50 $VC/ra/cave_ra.o -o $VC/ra/cave_ra.elf
    python3 $T/patch_ra.py $VC/c2.dll $VC/ra/cave_ra.elf $VC/c2ra.dll
    cd dev/lab/s6/target   # needs dsound.lib in cwd (the 10-15 image has none; copied from claude_client/target)
    docker run --rm -e RA_FN='?CG_DrawInformation@@YAXH@Z' -v $PWD:/w -v $PWD/build/vc:/vc -w /w \
      cod2-msvc71-vs2003 sh /vc/run_ra.sh $(id -u):$(id -g) objs.rsp
    python3 $T/ra.py build/ra.txt

`RA_FN` (mangled name) reaches c2 through wine's environment; the cave reads it with getenv once.
Hooks: F = 0x107724f6 (compile start, arms logging when `[sym+4]` == RA_FN), N = 0x10784d3b
(color.c node free, `ebx` = node). Globals live in the zero .data tail at 0x108b1f00..08.
PINNED 10-18: the probe does not change output. c2ra.dll (RA_FN armed) and stock c2.dll on the same
s6 objs give identical exes, 1,863,680 B, PE timestamp masked (`build/vc/run_stock.sh` -> build/os.exe).
The N line now also dumps 24 dwords of the object and 16 dwords of `[obj+0x28]`.

## What is PINNED

- color.c ICE line numbers (push line / push 0x1087ac40 / call 0x107fe722): 1221, 1497 in 0x107844a4;
  4532, 4778 in 0x1078805c (Ghidra start; reached by jmp 0x107c2613); 6773 in 0x10784d3b; line 6788
  in 23 other fns = the inlined id lookup `node = hash[id & 0x3ff]` (0x108807b8, chain +0x30,
  key +0x1c; stand-alone copy 0x1078ab34). c2 is PGO-split, so Ghidra's fn boundaries there are poor.
- Node = 0x48 bytes (0x10784d3b zeroes 0x12 dwords, puts it on free list 0x108a92d8):
  +0x00 object pointer, +0x14/+0x18 worklist links (head 0x108807b4), +0x1c id, +0x20 owned bitvector,
  +0x30 hash chain, +0x10 = `1 << k` while 0x1078b2aa processes nodes 32 at a time, +5 bit 2 flag.
- Object: +4 kind byte, +8 points back to the node. 0x1078002f returns 1 for kind 0x0d, else 2
  (0x1071d6d4 adds that up over a bitvector and averages it).
- CORRECTED round 3: kinds 0x04/0x05 are local/param leaf objects (sym+0x10 points to them), not
  constants; +0x1c is a small per-fn index. The "no const-0 node" reading below is void.
- c2 hidden options table (name, var, flags) at 0x10866xxx: `-db#` -> 0x108a9570 (no code reads it),
  `-NoLUR` -> 0x108a953c (read at 0x1070a4b0, 0x1070a7f3).

## Round 5 (10-21, ora): the colour choice is FOUND

PINNED (disasm + Ghidra decompile of c2.dll, confirmed by the probe log against our asm):

- Driver 0x107821a7: `for (cls = 6; cls >= 0; cls--)` over worklists 0x10882158[cls] (cls 0 first runs
  0x1078448d, then the same body). Per class: 0x107844a4, 0x10782473, 0x10782978, 0x1078482f build the
  sorted worklist; the pop loop at 0x10782a4d calls, per accepted node (n16 > 0):
  0x1078368e (walk the node's tuples, build its free-register set) -> **0x107833f2 (pick the register)**
  -> 0x107831f7 (update neighbours).
- **0x107833f2(edx = neighbour set, node, cls)** = the decision:
  1. `cost[0..0x4a] = 0` (array 0x10880680, indexed by c2 register id).
  2. For each neighbour node in the set with n16 > 0: `cost[r] += cnt` for each (reg r, cnt) pair on the
     neighbour's affinity list (node +0x38, pair = {next, reg object (+0x1c = reg id), count}) when r is in
     the neighbour's free set; if the neighbour has exactly ONE free reg r: `cost[r] += 100 * n16(neighbour)`.
  3. For the node's own affinity list: `cost[r] -= cnt` (copies/moves to r make r cheaper).
  4. Scan the class order table 0x1086917c[cls] (cls 0 = 0x108691a0 = ids 1,2,3,7,8,4,6), keep the first
     reg in the node's free set (node +0x20 bitvector), replace only on STRICTLY lower cost
     (0x10783499 `cmp cost[r], best; jl`). **Ties go to the earlier reg in the table.**
  5. node +0x10 = 0x108a97a4 + id * 0x60 (the register object).
- c2 GPR ids = Intel number + 1: 1 eax, 2 ecx, 3 edx, 4 ebx, 6 ebp, 7 esi, 8 edi (6/7/8/3 PINNED from
  G_GetPlayerViewOrigin: _origin -> 7 = `[esi]`, _client -> 8 = `[edi+..]`, bg_bobMax -> 6 = `mov ebp,ds:..`).
  Class-0 try order: **eax, ecx, edx, esi, edi, ebx, ebp**. Bits 0x40/0x80/0x100 tested in 0x1078368e =
  ebp/esi/edi (callee-saved). Other classes: 1 = ids 31..36, 5 = 58..65, 6 = 66..73 (x87/byte, unchecked).
- Free set excludes eax/ecx/edx for a node live across a call (seen on every call-spanning local).
- CORRECTION to round 4: worklist insertion 0x10784bba..0x10784bd2 advances while `new.n3 < cur.n3` or
  (`==` and `new.n17 < cur.n17`), so equal n3 pops the LARGER n17 first (not ascending). Split pieces
  copy n17 from their parent (0x1078b499..9f, 0x1078bf4d..53).

Probe: `cave_rd.s` (F + P + new C hook 0x107834a8, N dropped to fit the 688-B cave), patched by
`patch_ra.py` (hooks caveC if present) into lab/s6/target/build/vc/c2rd.dll; driver /home/nawaf/mlw/ora/rd.sh
TAG RA_FN (links claude_client/target objs; log ora/out/TAG.txt); `cpick.py TAG.txt` prints, in pop order,
each node, its n3/n16/n17, the free regs with their cost, and the pick (`*`). C line = node id, pick, best
cost, free mask (first 32 ids), cost[1..8], name.

### Validation: CG_DrawCursorhint 0x4bf920 (100 lines; retail fadeColor=ebx, text=edi, fontscale=ebx)

Ours (pop order): rect -> ebp | **fontscale piece (kind05 val=21, n3 48, n17 204) -> edi (free edi:0 ebx:0)**
| **text (n3 48, n17 116) -> ebx (only ebx left)** | font -> esi | ... | fadeColor -> edi (only edi left).
So the whole edi/ebx rotation is ONE tie: text and the fontscale piece have equal priority n3 = 48 and
the larger n17 pops first. If text pops first it takes edi (edi before ebx in the order table), the
fontscale piece gets ebx, and fadeColor (interferes with text, not with the piece) gets ebx = retail.
HYPOTHESIS: retail has text.n3 > fontscale-piece.n3 (weight differs by >= 1) or text.n17 > 204.
The cost array is all 0 for these (no affinity), so the register choice itself is not the lever; the
pop ORDER (n3, then n17) is.

Diagnostic tried: inverting the n17 compare at both insertion sites (0x10784bc0 jae->jbe, 0x10782b54
jb->ja, global) ICEs (main.c 148) in ui_shared_mp, so a global flip breaks another invariant. Next time arm
it per fn (test CUR in a cave) to check that the flipped order alone gives retail's CG_DrawCursorhint.

NOT register ties (PINNED, fdiff 10-21): G_GetPlayerViewOrigin (`push` placed after fstp), CG_FireWeapon
(`lea ebp,[ebx+0xf0]` between pushes), Item_ListBox_MouseEnter (one `mov edx,[esi+0x14]` moved) are
same-register instruction ORDER diffs = the scheduler, not color.c. G_GetPlayerViewOrigin's 7 colour
picks are all cost-0 first-in-order picks.

Next:
1. Find what sets n17 at node creation (writers above copy it; the original source is not found) and which
   source shape changes text's n3/n17 vs the fontscale piece. n3 = base + use weight (round 2), so any
   one-unit weight change on either node flips this tie.
2. Run `cpick.py` on the other register-rotation fns (CG_DrawAdsOverlay, Com_ProcessSoundAliasFileLocalization,
   XAnimLoadFile, mixer/Record fns); expect each rotation to be one equal-n3 pair.

## Round 12 (10-21, ora): whole-client triage (lab/w4/tri.tsv)

- `RA_FN='*'` now arms every fn (cave_rd.s caveF): ONE LTCG link (~13 s, 24 MB raw, 6,655 fns) gives all
  colour picks; `split_all.py all.txt outdir names...` + `ties.py` per fn.
- `tools_oracle/tri.py all_scored.txt` (run in target/) classifies each unmatched fn statically: B = same
  instruction multiset (order only), A = equal after consistent register renaming, A+B = same mnemonics,
  C = different; then refined by hand with the pick logs (fix_tri.py classes in tri.tsv).
- Result over the 60 unmatched named fns (38,3 KB): pop-order register ties (A) = only CG_DrawCursorhint
  (fixed by dt3, residue is operand order) and Window_Paint (lock-step pieces, parked). The other
  register-looking residues are NOT colour pop order: operand commutation from IL numbering (C-num:
  nb_decode, CG_WhizbySound, CG_DrawPlayerCompass, FX_RandomizeRoll), stack-slot layout (D-slot:
  FX_CalcOriginAndAxis, DSound_UpdateSample), short temps picked by the local code generator, not color.c
  nodes (D-local: FX_AddFxToScene `lea` temp, Menu_SetNextCursorItem, CG_DrawCrosshairNames).

## Round 11 (10-21, ora): Window_Paint under the formula

PINNED (winedbg watch on the three picked nodes h 0x0679200c, y 0x0679212c, w 0x06791e14 (+0xc), mk5.sh /
wv.py): w, y and h receive the SAME n3 update in every region (0x1078292d adds 10/17/24/30, 0x10783a80
subtracts), in lock-step, ending at 30 each. No region references one of the three without the others,
so the formula cannot separate them; the order is decided by n17 alone (82 w < 83 y < 84 h, the reverse
of the struct-copy def order h, y, w, x). Only a change in that def order (or a region touching one field
alone) can flip it; the 4 copy spellings and field-wise init (rounds 6-7) do not. Parked.

CursorHint residue after dt3 (36 lines, `rect->h + scale` fadd order + one push schedule): const-dropping
casts `((rectDef_t *)rect)->h` at one/both sites and `((rectDef_t *)rect)->w` = 36, unchanged (go.sh de1-4).

## Round 10 (10-21, ora): n3 writer FOUND (winedbg watchpoint)

Method (works, reuse it): heap addresses are identical with and without the debugger, so take the node
address from a probe run ([esp+0x20] at caveC, or P-line dword 2 = node+8 self pointer), then run the LTCG
link under winedbg with c2dbg.dll (cave_rd.s + `int3` right after `mov CUR,1`, build /home/nawaf/mlw/ora/
build_dbg.sh) and a command file `cont; watch *ADDR; (cont; x /1d ADDR; info reg) x N` (mk3.sh). Driver
/home/nawaf/mlw/ora/dbg.sh CMD RA_FN (DLL=c2dbg.dll; TERM must be set; the debuggee's stderr is lost, so
probe lines do not appear under winedbg). CursorHint: text node 0x067a4708, fontscale piece 0x067a4828.

PINNED n3 (node +0x0c) for these nodes is built in 0x107828e4..0x10782933 / 0x10783a31..0x10783a7b, per
region (block): walk the region's candidate list (+0x14 next), count the candidates with flag 0x10
(referenced here) -> `edx = count * [esp+0x24]` (region weight); then each flagged candidate gets
`n3 += [node+0x18] * edx` (its reference units in the region; then +0x18 = 0, flag cleared) and each
unflagged one gets `n3 -= edx` (0x10783a72..7b, skipped when esi == 2).
- text: +2 +2 +2 +2 (edx 1: the four `text =` defs, 2 units each) +4 +4 (edx 2: the `text && *text`
  tests) +32 (edx 8, 4 units = 2 uses in the draw block) = 48.
- fontscale piece: one event, +48 (edx 8, 6 units = its 3 uses in the same draw block).
- Units look like 2 per reference (HYPOTHESIS, matches both nodes). edx 8 in the draw block = number of
  referenced candidates there x region weight.
- So the tie is 4 x 8 + 16 (text) vs 6 x 8 (fontscale). One-point flips: one more text reference or one
  fewer fontscale reference in the draw block (+-16), or a different count of referenced candidates in a
  region where only one of them is referenced. Retail's code is identical apart from registers, so the
  difference must be IR-level (a reference that is optimised out later, or a candidate count).
- APPLIED (fast relink + cpick, CI): in the `cursorHintIcon == 3` arm, split the call into a local
  `str = UI_SafeTranslateString("PLATFORM_PICKUPHEALTH"); text = UI_ReplaceConversionString(str, binding);`.
  That def region now has 2 referenced candidates (edx 2): text n3 48 -> 50, pops before the fontscale
  piece, takes edi; fontscale piece ebx, fadeColor ebx = retail's whole edi/ebx rotation. fdiff 100 -> 36
  (rest = the known `rect->h + scale` fadd order and one push schedule). CI ora1 (lab/ora/ora_dt3.patch vs
  comment control ora_ctl.patch): both 1,322,134, NET 0, RATCHET OK (fn not yet byte-matched, so no bytes).
- Dead (pick unchanged, 100): `if (cursorHintString >= 0) text = ...; else goto`, `if (!text) goto; if (*text)`,
  `float fs = fontscale` copy.

## Round 9 (10-21, ora): n3 writer hunt (not found yet)

- wc.py (writes to [r+0xc] with a [r+0x40]/[r+0x44] access of the same base within 10 insns, whole .text)
  finds ONLY the four round-6 weight sites (0x10711011, 0x10711256, 0x10711790, 0x10711b46). None of them
  fires for rect/text/fontscale/fadeColor (CursorHint), so those nodes get n3 by a write with no nearby
  n16 access: a copy, a struct copy, or a computed store elsewhere.
- wc2.py 10780000 10790000 lists the 19 other `[r+0xc]` writes in color.c; untested candidates:
  0x1078292a / 0x10783a56 / 0x10783a7d (`ebx += [eax+0x18] * edx; [eax+0xc] = ebx`, then `-= edx`) and
  0x1078afa6 (`[esi+0xc] = [edi+0x40]`). The 688-B cave is full; the next probe needs a second cave (or
  winedbg `watch` on the deterministic node address from the P line, e.g. text 0x068c8dcc in rd.sh dch).
- Data for a formula fit (n3/n16 at pop): CursorHint rect 175/34, text 48/16, fontscale piece 48/4,
  font 24/2, temps 14/6 13/6 13/6, cg_cursorHints 9/2, fadeColor -6/8, hintIcon -12/2; Window_Paint
  w_param 494/170, pieces 48/6 30/6; SND temps 56/14 48/14 42/16 41/14 35/16 28/8 26/8 24/6. No single
  n16 multiple fits (HYPOTHESIS: n3 = savings over the live range minus a span/boundary cost).
- Window_Paint pieces: object vals h=24, w=387, y=389 (x not logged) vs def order h, y, w, x at the struct
  copy: def order is not object creation order. Not resolved.

## Round 8 (10-21, ora): breadth check, which swap fns are pop-order ties

PINNED (fdiff vs retail + cpick/pops on our build; rdv2.sh = rdv.sh with RUN=run2.sh for non-oc TUs):
- XAnimCalc 0x4888a0: main 191 lines = structure (dObj/count kept in different places, not a swap).
  With w87_ac (lab/w87_ac.patch, fresh third-loop var) 15 lines: one `[ebp+4]` load + movsx placed two
  insns earlier, with edx/eax temp names swapped = scheduling of one load pair, no named-var pop tie.
- UI_DisplayDownloadInfo 0x532830 (105 lines, 418 vs 415 insns): ours keeps the idiv result in ebx (retail:
  memory [esp+0x10], reloaded into ecx) and ours has an extra `mov ecx,esi; call; mov ebx,[esp+0x20]`
  block before a jmp (retail jump-threads it). Register-vs-memory + structure, not a pop-order tie.
- SND_RestoreStreamChannel 0x44eb60 (169 lines, 253 vs 250): retail pAlias0 in memory [esp+0x14] then
  edi, pAlias1 in ebx; ours gives pAlias1 ebx (only free reg, cost 2601) and pAlias0 pieces edi/eax/ecx in a
  second worklist (fl 2c, n3 26/16/14). The pieces are split differently (insn count differs), so it is
  splitting/weights, not an equal-n3 tie.
- VM_Notify 0x47e200 (62 lines, 444 vs 438): one temp edx vs eax (`lea r,[eax+eax*4]`) plus 6 insns of
  structure; not a named-var tie.
- So of the swap list only CG_DrawCursorhint and Window_Paint are pure equal-n3 pop-order ties (round 7).

## Round 7 (10-21, ora): where n17 is born

PINNED (w44.py scan of every `mov [r+0x44], r2` + probe hook W at 0x1077fadc, cave_rd.s):
- 0x1077fa92..fae3 walks the fn's IL tuples in order with a counter ([esp+0x1c], `inc` per tuple at
  0x1077fa99). For each operand with kind byte 1 and [op+0x1c] == 0x108a92e4 (not yet bound), it binds
  op+0x1c = node (node = table[[op+0x10]]) and writes **node.n17 = tuple counter**. Rewritten on every such
  operand, so **n17 = IL position of the node's LAST bound reference in the walk** (defs: opcode 1 = assign,
  618 = load, 99 = store, 18 = ?). Split pieces then copy n17 from the parent (round 5).
- CG_DrawCursorhint: text (id 12) bound at tuples 104, 107, 112, 116 (its four `text =` defs, opcode 1);
  fontscale piece (id 16) at 204 (opcode 618, the param load before UI_TextWidth). Every text def comes
  before that load, so text can never get n17 > 204 by def placement; the lever is the fontscale load
  position (< 116) or the n3 values (both 48, origin still unknown).
- Window_Paint: fillRect pieces are defined by the struct copy at tuples 9, 11, 13, 15 (ids 14 h, 27 y,
  25 w, 23 x) and reloaded after the debug UI_DrawRect join at 81..84 in REVERSE order (x, w, y, h). The
  same reverse store/reload pattern shows at 85..105 (stores 3,10,40,5,7 opcode 99, loads 7,5,10,3).
  So n17(w) = 82 < n17(y) = 83 follows from the struct-copy def order (h, y, w, x). Retail needs the copy to
  define w before y.
- Dead (identical pick and 42 lines): `fillRect = w->rect[0]`, memcpy, init at the declaration. Field-wise
  init (round 6) breaks the struct pieces (258+ lines).
- Next: find what orders the struct-copy defs h, y, w, x (not field order; piece object vals h=24,
  w=387, y=389 suggest creation order of the scalar pieces), and the n3 origin for the CursorHint pair.

## Round 6 (10-21, ora): what moves n3/n17 (CG_DrawCursorhint)

Tools: /home/nawaf/mlw/ora/rdv.sh (go.sh + c2rd.dll + RA_FN: per variant var/<v>.diff and ora/out/<v>.pick),
`ties.py log` (marks N3TIE = equal n3 with the previous pop, ORDER = several free regs at the lowest cost).
cave_rd.s now also logs A/S lines: priority add 0x10711b36 / subtract 0x10711780 (node esi, amount
eax*ebp, ebp = block weight); acc.py sums them per node. (The 0x10711249 / 0x10710ffb pair of round 2 and
these two are the only 4 code sites writing node +0x40 and +0x0c together, scan pair.py.)

PINNED (probe, 8 builds):
- None of the 4 weight sites fires for the text (id 12) or fontscale-piece (id 16) nodes, so these two
  priorities are not the per-use weights of round 2. (CORRECTED round 7: 0x1078ac92 restores a different
  struct, its "+0x40/+0x44" are pointers; it is not the n16/n17 record.)
- No effect on (n3, n17) of text / fontscale piece (all still 48/116 and 48/204, fdiff 100):
  local decl order (scale first/last, text first/last), `float fs = fontscale` copy (object id 21 -> 22,
  n17 unchanged = n17 is NOT the object id), `if (!text || !*text) goto draw_icon;`, `if (text) if (*text)`.
- CG_DrawAdsOverlay 0x4c26e0: no N3TIE pick at all = its residue is scheduling (as w86 said), not colour.
- Com_ProcessSoundAliasFileLocalization: the f problem is weight (n16 <= 0, round 4); its three N3TIE picks
  (temp val=691, len, buffer) are not the diff.
- Window_Paint 0x533bd0: the w/h/y restore (retail edi=w, esi=h, ebx=y; ours ebx/esi/edi) = three kind04
  fillRect pieces with EQUAL n3 30 popped by n17 84 (h) -> esi, 83 (y) -> edi, 82 (w) -> ebx. Retail
  order h, w, y needs n17(w) > n17(y). Field-wise fillRect init in 4 orders (xywh/xwyh/hwyx/yxwh): code
  258-262 lines (main 42), pieces no longer coloured = dead form; the struct copy must stay.
- Per-fn tie table (stock build, ties.py):

| fn | colour diff? | deciding tie (ours) | flip needed |
|---|---|---|---|
| CG_DrawCursorhint | yes edi/ebx | text vs fontscale piece, n3 48 = 48, n17 116 < 204 | text first (n3 +1 or n17 > 204) |
| Window_Paint | yes w/y | fillRect w vs y pieces, n3 30, n17 82 < 83 | n17(w) > n17(y) |
| SND_RestoreStreamChannel | yes ebx/edi | no N3TIE among pAlias picks; `_name` N3TIE (n3 24) only | not a pop tie: cost/free-set (unread) |
| UI_DisplayDownloadInfo | ebx at join | width/downloadSize n3 48 tie, text 32/28 ties | unmapped to retail regs |
| XAnimCalc | partly fixed by w87_ac | refStr n3 94, bufferIndex 72, parts 45 ties | unmapped |
| VM_Notify | edx vs eax temp (+6 insns other) | many N3TIEs (parentValue/kind04 320, selfNameId 224 ...) | unmapped |
| CG_DrawAdsOverlay | no | none | scheduling, not colour |
| Com_ProcessSoundAliasFileLocalization | f in memory | weight n16 <= 0 (round 4) | not a tie |
| FX_CalcOriginAndAxis | not probed | stack-slot shift, not register (LESSONS ad) | - |

- OPEN: where n17 is born. It is not the object id, not decl order; split pieces inherit it; consecutive
  values for fillRect pieces (82..84) say it is a creation counter. Next: hook node allocation 0x1077c10f's
  callers / the record at 0x1078ac92 to log who sets [rec+0x10], then map it to IL order.

## Round 4 (10-18): the allocation queue, Com_ProcessSoundAliasFileLocalization

PINNED decision site. The allocator pops color nodes from a worklist (head 0x108807b4, links +0x14 next,
+0x18 prev) at 0x10782a4d. Insertion at 0x10784bba..0x10784bf4 keeps the list sorted by node +0x0c
(n3) descending, with ties broken by +0x44 (n17) ascending. Right after the pop, 0x10782a5e:
`mov eax,[node+0x40]; cmp eax,0; jle 0x107896ae`, so a node with n16 <= 0 is rejected (no register
for that piece). Insertion at 0x10784b8f also requires n16 > 0. So n3 = priority, n16 = gain, and the
gain must be positive.

Probe: `cave_ra.s` now also hooks 0x10782a5e (P line = node + object + sym name).
`build/vc/ra/pops.py build/ra_snd3.txt` prints the pop order. Output identity re-checked: ox.exe ==
os.exe (stock c2) apart from the PE header.

CORRECTION to round 3: the register-only locals DO have color nodes (i, token, len, file, hasName, data,
buffer are all popped). The 0x10784d3b free hook just never saw them. Kind 0x03 objects are compiler
temps (val = temp number, ~200-1150).

Pop order, ours (94 pops, excerpt, `fl` = node +5):

| # | node | n3 | n16 | result |
|---|---|---|---|---|
| 1-45 | phase 1 (fl 0c/04/84): temps, i (216/80), token (158/72), pi, len, c, data (69/52), file (26/6), buffer (24/2) | | >0 except 41/43/44 | registers |
| 41 | kind 08 object val 0x3e, chained to f's leaf (f's first piece, from the call to the loop) | 16 | -2 | REJECTED (n16 <= 0) |
| 45 | hasName | -84 | 16 | accepted (last in phase 1) |
| 46 | lineStart piece (fl 2c) | 190 | 8 | accepted (ours keeps lineStart in ebx) |
| 47 | f piece | 104 | 0 | REJECTED |
| 57 / 58 | f pieces | 16 / 12 | 0 / 2 | rejected / accepted (tiny piece) |
| 72 / 74 | the same kind-08 f node, retried | 8 / -979 | -2 | rejected |

So f never gets a register in ours on weight alone: every f piece that covers the call site or the loop
has n16 <= 0. That holds whatever esi is holding. The string constant sitting in esi past the call is a
consequence: esi is still free after FS_FOpenFileWrite (which keeps esi), so the CSE temp of the
literal keeps it. In retail, f's first piece must have n16 > 0; it takes esi (`mov esi,eax`) and the
constant gets rematerialised.

Weights: f's first piece is 16 / -2 against the accepted temps above it (n16 2..80). It is short by
3 gain points. Gain = per-use value * block weight (0x10711244) minus 1 << cl per boundary
(0x10711005).

Source change to test (HYPOTHESIS, unbuilt): add gain to f's piece between the call and the loop
without new code. Retail's warning line pushes the literal as an immediate, so try making the warning
NOT reuse the call's argument value: give the warning its own spelled-out literal that cannot pool with
the argument, e.g. `Com_Printf("WARNING: Could not open output file %s for writing\n",
"soundaliases/temp.csv")` with the argument written through a separate `static const char` array, or
the reverse. If the CSE temp disappears, f's piece loses a register competitor at the call boundary
and its boundary cost may drop. Check with `pops.py`: the kind-08 f node must show n16 > 0 and be
accepted. If it still shows -2, the gap is in f's own use count in that piece, and the next variant is
the `if (!f)` test shape (`if (f == 0)` vs `if (!f)` vs `if (f <= 0)`).

## Round 3 (10-18): snapshot diff, Com_ProcessSoundAliasFileLocalization

Tools: `cave_rb.s` (patched with `patch_ra.py` into build/vc/c2rb.dll, run with `build/vc/run_rb.sh`) writes
every committed private RW region to build/ra1.bin at the armed fn's compile start, build/ra3.bin at its
first color-node free, and build/ra2.bin at the next fn's compile start (~50 MB each, deterministic heap
addresses across runs). `build/vc/ra/symtab.py aliasOSPath build/ra1.bin build/ra3.bin build/ra2.bin`
prints the locals; `ra.py build/ra_snd.txt build/ra1.bin aliasOSPath` names the color nodes.
`cave_rc.s` (logs the 5 `and [sym+0x20],~0x800` sites) fired 0 times for this fn, so those sites are not
where locals are promoted.

PINNED local symbol record (S = record, name inline before it):
`+4` name ptr, `+8` fn scope, `+0x10` leaf object (the color.c node object, kind 4 = local, 5 = param),
`+0x18` frame offset, `+0x1c` size, `+0x20` storage (0x900 = scalar with frame home, 0x2900 = array/
addr-taken, 0x100 = no memory home), `+0x28` sym id, `+0x30` 1 -> 0x50001 when the frame slot is final.
By the next fn: `+0x10` cleared, frame offsets repacked (f -2156 -> -28032).

PINNED outcome (ra2 + ours' disasm): register-only (st 0x900 -> 0x100, home dropped): token, i, file,
len, localize, hasName, lineEnd. Kept in memory with a new frame slot: f ([esp+0xc] right after
FS_FOpenFileWrite, every FS_Write reloads it), columnCount, localizedCount, lineStart, hasFile, plus the
address-taken data/buffer and the arrays. Params loadspecCurGame/sourceFile go 0x100 -> 0x900 before
the first color-node free (they get their stack home). In ours esi holds `i` (`inc esi; cmp esi,0x18`),
edi holds zero, then `token` (`mov edi,eax` after Com_Parse). The register numbers are not in the
symbol record (only the 0x100 storage flag); they live on the temps / IL operands.

(VOID, see round 4) the 7 register-only locals have NO color.c node of their own (their leaf objects never appear).
Only the memory-bound locals and params do, along with 41 compiler temps. So the locals are split in
two before colouring: promoted to temps (then coloured) or kept as memory candidates.

Color nodes per memory-bound local, in free order, as `[node+5 flags  n3/n16]`:

| local | n17 | nodes |
|---|---|---|
| f | 60 | [0e -657/58] [2e -360/44] [0d -16/-12] [6e -979/0] [6e -979/0] [0d 4/0] [0d 8/0] |
| columnCount | 181 | [0e -727/66] [2e -482/36] [0d 10/0] [6e -979/0] [0d 24/0] |
| localizedCount | 407 | [0e -919/14] [2e -574/8] [0d 10/0] [6c -979/-8] |
| lineStart | 134 | [06 -382/36] |
| hasFile | 178 | [0e -88/16] [6e -979/-20] [0d 16/0] |
| loadspecCurGame (param) | 34 | [0e -891/6] [2e -546/2] |
| sourceFile (param) | 33 | [0e -807/20] [2e -486/12] [0d 8/0] [6e -979/-4] [0d 24/0] |

Reading (PINNED arithmetic, meaning HYPOTHESIS): n3 and n16 get the same += / -= (0x10711244 /
0x10711005), so n3 - n16 is a per-candidate base fixed before use weighting: f -715, columnCount -793,
localizedCount -933, lineStart -418, hasFile -104. n16 = the use weight (f 58, 2nd highest). n17 is not a
weight: the two params are 33/34 and the 501/502 leaves are 264/265, so it is a sequence number. Every
memory-bound candidate stays below 0 on its first nodes and all of them ended in memory. Best-fitting
rule: a memory candidate is enregistered only if use weight beats the base cost, and f's base (-715) is
far beyond any use count.

Which source change flips f into esi (HYPOTHESIS, untested): the register-only locals are the ones not
live around the outer `while (data)` back edge; f, columnCount, localizedCount and hasFile are. So
retail's f most likely goes the promoted-temp path: f's live range is shorter or simpler there (for
example the file opened or closed at a different point, or the loop shaped so f isn't a loop-carried
memory candidate), not more uses. Adding uses moves n16 by tens, against a base of -715.

Next: find the promote-to-temp pass (writer of st 0x100 for token; its 0x900 -> 0x100 happens after
the first color-node free) and its criterion; then test one f live-range variant in a lab copy and
check that f loses its color nodes.

## Round 2 (10-18, PINNED unless tagged)

- Node +0x0c and +0x40 are weight accumulators. Add: 0x10711237..56 `eax = f(op) (0x10711268) *
  [esp+0x18]; [node+0x40] += eax; [node+0x0c] += eax`. Subtract: 0x10710f82..1011 `[esp+0x18] = 1 << cl;
  ... [node+0x40] -= it; [node+0x0c] -= it`. Values start near -1000 (0xfffffc2d = -979 is common),
  so +0x0c reads as benefit minus a fixed cost (HYPOTHESIS: cl = loop-depth scale).
- 0x1078b346 is liveness, not colouring: for each block it ORs node `+0x10` bits into GEN/KILL-style
  masks stored at block +0x2c/+0x30/+0x34/+0x38/+0x58/+0x5c. 0x1078b2aa runs it over 32 nodes at a time.
  So the 0x1078b2aa / b346 / b51c / b79c family is dataflow; the register choice is not there.
- Object kinds: 0x0d = compiler temp (`+0x1c` = 0, `+0x28` = defining tuple, opcode word at tuple +4:
  0x24b / 0x24f / 0x251); 0x04 / 0x05 = local / param leaves (see round 3; 0x07 unconfirmed); 0x03 and 0x08 have small ids at +0x1c (0x279 / 0x28b / 0x2a9, 0x3e), meaning
  unknown.
- Com_ProcessSoundAliasFileLocalization: 122 nodes = 41 temp nodes, constants, 3 kind-0x03 and 3 kind-0x08.
  None is a named local, so `f` is not in this structure and no f-vs-esi table comes from this hook.
  In CG_DrawInformation, 300 / 320 / 9 are never register-held in ours (they are float/immediate uses),
  so these nodes are not "constant in a register" candidates either.
- Not found yet: where a named local gets its register. Source change to flip `f`: UNKNOWN. The
  weight formula above makes "more uses of f inside the parse loop" the best guess (HYPOTHESIS);
  decl order does not enter it.

## HYPOTHESIS (round 1, partly superseded above)

- node +0x0c (n3) and +0x40 (n16): small signed values (2, -1, 0, 12, -157, 4, 8, 0x10, 0x18,
  -979 = 0xfffffc2d repeated). Could be a register code, a frame offset, or a cost delta.
- node +0x44 (n17): same value for every node of one object (320 -> 271, 9 -> 258, 300 -> 179):
  per-candidate weight?
- CG_DrawInformation (ours): 54 nodes, 41 kind-0x0d (vars/temps), constants above, and NO node for
  constant 0. If these nodes are register candidates, ours never makes 0 a candidate, so no zero
  register is possible. Unconfirmed: the probe only sees nodes freed via 0x10784d3b, and this pass
  may be live-range splitting or rematerialization rather than the coloring.

## Next attack (after round 2)

1. Find the allocator over named symbols: the fn sym's local list (`[sym+...]`, fn name at `[sym+4]`),
   then the writer of the register field. Try a write breakpoint under winedbg on a known local's
   symbol struct, or dump every local sym (name + all dwords) at compile start and at the next F, and
   diff the two snapshots: the field that changes to esi/edi/ebx codes is the assignment.
2. Callers of the weight code (0x10711005 / 0x10711253 sit in Ghidra fns near 0x1071128c / 0x107115a7)
   lead to the pass that consumes +0x0c.

## Next attack (round 1)

1. Name the kind-0x0d objects: dump object +0x28 and deref it (the sym name pointer in fn syms is
   `[sym+4]`). Then compare `f` in Com_ProcessSoundAliasFileLocalization (retail esi, ours stack).
2. Find the colour choice: hook the writers of node +0x0c / +0x40 (scan for `mov [reg+0xc]` and
   `[reg+0x40]` stores near the 0x1078xxxx color.c fns), or hook 0x1078b346 / 0x1078b51c / 0x1078b79c,
   which 0x1078b2aa calls per 32-node group.
3. Validate on a fn where retail and ours both hold a known var in a known register.
Helpers: `$VC/ra/c2d.py c2.dll VA n` (disasm), `$VC/ra/xref.py c2.dll VA` (call/jmp sites),
`$VC/ra/out/*.c` (Ghidra decompiles of the 28 assert-site fns), Ghidra copy `$VC/ra/proj`.

## 10-25 lever study (rs_, SND_RestoreStreamChannel)

Driver: /home/nawaf/mlw/ora/rs_rdv.sh (= rdv.sh with src -> mlw/rs_src, a fresh copy of target/src, and
VC -> /home/nawaf/rgb/vc; lab/s6/.../c2rd.dll is now an empty dir). One TU compile + LTCG link, ~17 s per
variant, several in parallel. Variants in mlw/var/rs_*.cpp, picks in mlw/ora/out/rs_*.pick.
- Ordering field = n3 (node +0x0c), ties by n17 (+0x44), as rounds 5/10. Nothing new orders colouring.
- Retail order (types then blend, rs_t1) = 169 lines; pAlias1 (one node, n3 15, n16 30) pops in phase 1
  with ONLY ebx free (cost 2601): ebp memFile, esi channel, edi kind03 val=555 (n3 56, n17 268). pAlias0 is
  split into 3 phase-2 pieces (n3 26/16/14) and takes edi. Not a tie: changing pAlias1's register needs
  val=555 or pAlias1 itself to lose/gain whole region weight, or pAlias1 to fail phase 1 and be split
  (retail spills both pointers to [esp+0x10]/[esp+0x14], so retail likely splits pAlias1 too: HYPOTHESIS).
- Moved only by reference placement: pAlias1 type check before pAlias0's (rs_w2) moves the pAlias0
  piece n3 26 -> 33 (its refs land in a heavier region), code worse (288 lines). pAlias1 n3 15 and
  val=555 n3 56 did not move in any of 8 variants: VectorCopy tail, cached offset ptr, rate inlined into
  the call, a "!= 0" test, "(flags >> 5) & 3", if/else arm swap (210 lines), and the earlier decl/chan/flag tries.
- Unresolved: what val=555 is (late IL, n17 268; not the tail chaninfo CSE, which the tail edits do not
  touch). CG_AddPlayerWeapon not reached this session.
