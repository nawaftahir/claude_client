# c2.dll instruction order (B-class residue) - hunt notes (10-21, ora)

Status: the scheduler is NOT found yet. Facts below are PINNED unless tagged.

- c2 has no scheduler source file in its ICE table (`strings` on vs70builds paths): list.c = asm listing
  writer (/FA, 0x1080acbb..), mdlist.c = its x86 part, dag.c anchors 0x107c381d/0x107c384a are small
  edge-flag helpers (`|= 0x80000` on a matching edge, else 0x107b418c), gzmd.c / mdmisc.c anchors build IL
  (prologue `__penter`, stack probes). Only "PEEP:" strings mention reordering (branch pairs, compares).
- 0x107ba533 walks a list of 12-byte-headed records (+0 kind: 0 / 10 / 0x14 special, +4 next) calling
  `[0x10881848]` per record and summing sizes into [edi+0x18]. Logged with cave_sc.s hook E 0x107ba622
  (c2sc.dll, rd2.sh with DLL=c2sc.dll): only 35,669 records for the whole client, so these are not
  instructions (fixup or debug records). Not the emission order.
- No `mov byte [..],0x68` in c2: the encoder is table-driven.

## Probe output identity (PINNED 10-21, /home/nawaf/mlw/ora/ident.sh)
Same claude_client/target objs, PE timestamp masked, vs stock c2.dll: c2rd.dll idle (RA_FN unset),
c2rd.dll armed with RA_FN='*', c2sc.dll armed: all IDENTICAL (1,863,680 B). (The earlier gpvo/gsc diff was
two builds of different main objs, not the probe.)

## Pre-RA tuple order (cave_rd.s hook T 0x1077fa92, one `A` line per tuple: ptr + 12 dwords)
PINNED tuple layout: +0 prev, +4 opcode, +8 type/flags, +0x10 next, +0x14 = line-ish counter, +0x28/+0x2c
operand lists. G_GetPlayerViewOrigin: 198 tuples logged; the push run before the call is tuples 43..54
(opcode 1 movs, 0x12 pushes) after the x87 chain 40..42 (0x63, 0x45, 0x60).
winedbg watch on that push tuple's +0/+0x10 (0x06b05b30/40): relinked by 0x1075d364/0x1075d371/
0x1075d88x/0x1075f0a3/0x1075f153 (IL optimisation), then the slot is freed and reused by other objects
(0x107afd98, 0x107b41cb, 0x107b9c6b ...). So the IL tuple does not survive to the final order: lowering
builds a different instruction structure and the scheduler (if any) works on that. Watching IL tuples
cannot catch it.

## IL relink sites of the push tuple (10-21 ora, disasm + Ghidra)
PINNED: 0x1075d364 = generic unlink (prev/next at +0/+0x10 cleared); 0x1075f093 = insert-after;
0x1075f25b = insert-before; 0x1075d885 sits in FUN_1075d6ad, which re-appends tuples at the global
cursor 0x108a3d88 (`p->next = cur->next; cur->next = p; p->prev = cur`) while expanding an expression node:
it allocates temps (op 0x24f), counts operands (0x29e = call with arg list, 0x29d = 3), and for binary ops
0x278..0x28e compares the two operand subtrees' +0x10 values (`a[4] < (*a)[0x10]`, then an equality
branch) before emitting them = operand evaluation order by a per-subtree weight (Sethi-Ullman-like,
HYPOTHESIS). This is linearisation (tree -> tuple list), not a post-RA scheduler; the tuple order logged at
0x1077fa92 is its output. winedbg `bt` is useless here (no frame pointers in c2).
HYPOTHESIS worth testing next: the C-num class (fld/fmul operand roles, base/index order) is decided by
this compare; log both +0x10 weights per binary op for CG_DrawPlayerCompass / FX_RandomizeRoll and look
for equal-weight ties.

## Round 3 (10-21 ora): the FUN_1075d6ad binary-op compare is NOT the operand-order decision
PINNED (Ghidra listing via tools_oracle/Lst.java, /home/nawaf/mlw/ora/lst.sh ADDR): the compare sits at
0x1070db53..60 (ebx = operand list [op+0x28], esi = [ebx] = 2nd operand, `cmp [ebx+0x10],[esi+0x10]`; w1 <
w2 -> swap path unless type class (word op+0xa & 0xf000) == 0x4000; equal or greater -> 0x1075f7ef, normal
order). Reached only via 0x1075f7e9. cave_rd.s hook O at 0x1070db58 (c2rd.dll; probe still output-identical
by construction, not re-measured) fires ONCE in the whole client with RA_FN='*', zero times for
CG_DrawPlayerCompass, CG_WhizbySound, FX_RandomizeRoll, CG_DrawCursorhint. So x87 operand roles are decided
elsewhere (candidates: lowerflt.c, the fp stack code, or the tree builder before linearisation).

## Round 4 (10-21 ora): operand order of float ops is not decided in c2's linearisation
PINNED:
- Main copy of the binary-op rule is 0x1075d7a0..ba (jump-table case for ops 0x278..0x28e): w1 = [op1+0x10],
  w2 = [op2+0x10]; only w1 < w2 swaps (path 0x107430bf), and that path is skipped for type class 0x4000.
  Hook O there (c2rd.dll, armed '*' output IDENTICAL to stock, ident2.sh) logs CG_DrawPlayerCompass:
  0x278 = int add (type 0x1004, weights 0x10060/0xc ...), 0x27a = fmul and 0x279 = fsub/fadd with type
  0x4004 (class 4 = float): float binary ops never swap here. "Weights" are packed (e.g. 0x0102047e).
- winedbg watch on the h-line fmul tuple's operand fields (+0x28/+0x2c): operands are appended by
  0x1075a485 and removed by 0x1075abe0 (list unlink); no swap site.
- Source: `h = rect->h * size` vs `size * rect->h`, and the y line `(size-1)*rect->h` vs
  `rect->h*(size-1)`, all build identical (go.sh pc0..pc3, 12 lines): the front end canonicalises the
  float operand order before c2 sees it. Retail loads size first for h but rect->w first for w, so the
  canonical key is something per-expression in c1 (HYPOTHESIS: c1 node/symbol creation order), not c2.

## Round 5 (10-21 ora): CG_ConfigStringModified add -> lea anchor
- Diff: ours `mov eax,[esi*4+offs]; add esp,4; add eax,data; cmp esi,7; jne`, retail `...; add esp,4;
  cmp esi,7; lea eax,[eax+data]; jne` (the str address add sunk past the cmp as a flags-free lea).
- A full-body tiny LTCG clone (tiny/gencf3.py d0, `?f@@YAXXZ`, tiny/ltc.sh) reproduces ours (num in esi,
  add before cmp). Unchanged in the clone and/or the real fn (go.sh cf0-3, 15 lines): CL_GetConfigString as
  `&data[off]`, `off + data`, a local base or offset, a char* cast, non-static helper, decl order,
  num/str assigned in one statement, str computed inline without the helper, `str[0]` for `*str`.
- Not yet found: the c2 code that sinks an add past a compare and rewrites it to lea (no string anchor;
  next: scan c2 for writes of the lea opcode into an instruction object, or watch the add tuple's opcode
  field under winedbg in the clone, which is fast to link).

## Round 6 (10-21 ora): watching the clone's add
- tiny/ilrun2.sh SYM t (probe IL dump of a tiny LTCG DLL link) on the gencf3 clone: 161 tuples. Entry
  block (blk 5): op 0x2d, 0x0e (call), 0x0d, 0x2d, 0x0e, 0x0d = the two arg/call/stack-adjust triples of
  `atoi(CG_Argv(1))`; blk 6 = 0x01, 0x2d, 0x01 (candidate add at 0x005096e0, HYPOTHESIS).
- winedbg watch (tiny/dbgt.sh + s5.sh) on that tuple's +0/+4/+0x10: after RA the slot is reused by other
  objects (late writers 0x107b2baa / 0x107b615b store byte +0x16 of a different object). Same lesson as
  round 2: the post-RA instruction objects are new allocations; IL tuple addresses cannot be followed
  into the scheduler. Opcode 0x2d is shared by the arg pushes and the candidate add, so the numbering
  is not x86-level either.

## Round 7 (10-21 ora): machine opcode numbering PINNED
- The /FA listing mnemonic table is at 0x108743a8 (used by mdlist 0x1082c3bf/0x1082c43b; mn2.py dumps it):
  internal machine opcode = index. 0x01 mov, 0x0d push, 0x0e call, 0x12 lea, 0x2d add, 0x2f cmp (and
  the neighbours: 0x2c adc, 0x2e and, 0x30 or, 0x31 sbb, 0x32 sub, 0x33 test, 0x34 xor, 0x0b pop,
  0x0f jcc(_jcc), 0x10 jmp, 0x26/0x27 dec/div ...). The tuple +4 field at colouring time uses this table.
- Re-read of the tuple walk with it: the walk visits blocks in reverse and tuples in reverse inside a
  block. ConfigString clone: entry block = push 1, call, add esp; push, call, add esp; next block = mov,
  add, mov; the cmp (0x2f) + jcc sit in a separate later block at colouring time. So the retail add->lea
  sink crosses a block boundary and happens after blocks are merged.
- Stores of opcode 0x12 into a tuple (scan): 0x1072be31, 0x1073f169, 0x1077733a, 0x1079f17b, 0x107e5075,
  0x107e57d9 (lowering), 0x1084a6e7, 0x1084a9b3, 0x1085e70c, 0x108621ae (mov->lea for a 0x250 operand).
  Checks for add 0x2d: 0x107196d2, 0x1078a10c, 0x107e9e44, 0x10860d14 (relink then branch on add),
  0x10862274. Hook M at 0x10860d14 (cave_rd.s) fired 0 times for the clone: not the sink.

## Round 8 (10-21 ora): lea stores probed
- /home/nawaf/mlw/ora/lea/mk.py builds c2lea.dll: F arm + one replay stub per `mov dword [reg+4],0x12`
  site (10 sites), logging all registers; tiny/ilrun3.sh DLL SYM t runs it on a tiny LTCG link.
- ConfigString clone: only 0x1073f169 fires (once), the address-folding builder: before it, `push 0x255`
  (address-mode operand) + base/index operands, after it `sar edi,1` (scale). It refuses (jumps to
  0x10735157) when the source operand is a register temp equal to the destination temp ([ebx+0x18] ==
  [ebp+0x18]). So an add becomes lea here only as a three-address form (dst temp != src temp); our
  `add eax,data` keeps dst == src. HYPOTHESIS: retail's IL had a fresh temp for str (t2 = t1 + data),
  ours reuses t1. Clone shapes for str (`str = data; str += off`, `&data[off]` via a local, int-cast add,
  `off + data`) keep `add eax` (tiny/gencf4.py e1-e4).
- Probe restored: cave_rd.s = T hook again; c2rd.dll armed '*' IDENTICAL to stock on the same objs.

## Round 9 (10-21 ora): the lea store has two entries; same-temp test not reached
- xr.py (direct-branch xrefs): 0x1073f169 is also entered by `jmp` from 0x1073064c, a second path that
  pushes operand kind 0x253 (instead of 0x255) and is reached from 0x1072bad6 / 0x1072e423 / 0x1072e42f /
  0x107358f4..0x1073590c. Ghidra has no function there (PGO split).
- Hook Q at 0x1073f13e (lea/mk2.py -> c2lea2.dll, replays cmp/jne) logged nothing for the ConfigString
  clone: the str add never reaches the same-temp test; it is filtered earlier (one of 0x1073f101 /
  0x1073f109 / 0x1073f11a / 0x1073f124, or never enters this builder). The round-8 "dst == src" reading
  is therefore unconfirmed (HYPOTHESIS).
- References: opencod2 CL_GetConfigString uses `clientActive_t *cl = &clients[0]; int offset = ...;
  return cl->gameState.stringData + offset;` and a switch in CG_ConfigStringModified (retail's compare
  order 7,8,3,0 follows our if-chain, not a switch). Clone helper shapes g1-g4 (cl/gs locals, offset
  local, result local): all keep `add eax,data`.

## Round 10 (10-21 ora): the add/lea choice is a lowering switch on OPERAND KIND (PINNED)
- 0x1073f0f7 (lea builder, kind 0x255) is a case of the switch at 0x107772db `jmp [eax*4+0x10777370]`,
  entered from 0x107739a8 with eax = operandKind - 0x24b. Table (case -> target): 0 -> 0x107e57bb (sets
  op = 1 mov), 5 -> 0x107e57cf (op = 0x12 lea), 6 -> 0x107e584a, 7 -> 0x107772e2, 8 -> 0x107358f1 (lea path
  pushing kind 0x253, enters the lea store via 0x1073064c), 9 -> 0x1072be21 (lea store 0x1072be31),
  10 -> 0x1073f0f7 (lea builder, kind 0x255). Cases 1-4 empty.
- lea/mk3.py -> c2sw.dll logs the switch (F arm + regs) on the tiny clone: two events for f: case 8 and
  case 7; the case-7 operand (ebp 0x004dfc9c) is the operand of the clone's `str` add (same pointer as in
  the T dump). So ours lowers the str add with an operand of kind 0x252 (case 7, stays `add`); a lea
  comes only from kinds 0x24b+5 / 0x253 / 0x254 / 0x255. Retail's `lea eax,[eax+data]` = its str operand
  had one of those kinds.
- Next: identify what operand kinds 0x250/0x252/0x254/0x255 mean (log the operand object fields for
  case 7 vs case 8/9/10 events across the client with RA_FN='*', using a crash-safe dump of the operand
  pointer that the switch already holds in ebp), then the IL/source shape that yields 0x254/0x255 for
  `data + off`.
- c2sw.dll / c2lea*.dll are diagnostic only (identity unchecked); c2rd.dll stays on the verified T hook.

## Round 11 (10-21 ora): case 7 makes add OR lea; retail's lea is same-register
- tiny/kinds.sh (8 one-line fns on the c2sw.dll switch probe): `data + offs[g]` -> case 7, `add ecx,data`;
  `&data[g]`, `(char*)offs + g`, `int o = offs[g]; use2(data + o, o)` -> case 7, `lea ecx,[eax+data]`
  (result in a different register from the input = three-address); `data + g*4` -> case 0,
  `lea ecx,[eax*4+data]`. So kind 0x252 (case 7) = register + symbol address; it becomes lea only when the
  destination register differs from the input.
- Retail's `lea eax,[eax+data]` uses the SAME register and sits after `cmp esi,7`: it is not the case-7
  three-address lea but the add moved past the flags-setting cmp and rewritten (a scheduler step).
  Our clone never moves it: /G5 /G6 /G7 /GB identical (tiny/gflag.sh), CG_Argv inlined with a static
  buffer like retail (tiny/gencf6.py j0) identical.
- Open: find the post-RA step that moves an instruction across a flags def and rewrites add->lea.

## Round 12 (10-21 ora): `flags-def; lea r,[r+imm]; jcc` is rare
- /home/nawaf/mlw/ora/leascan.py (linear capstone sweep + map) finds exactly ONE such site in our main exe
  (Con_AnySpaceAfterCommand 0x405092: `mov al,[esi+buf]; test al,al; lea esi,[esi+buf]; je`, matched) and
  one in retail (the same fn); CG_ConfigStringModified's retail site is not found by the linear sweep
  (sweep desync near jump tables, so counts are a lower bound).
- The Con site is a plain data-dependence order (the load must read old esi before the pointer update),
  not a free move, so it is not an anchor for the CG_ConfigStringModified case, where nothing forces the
  add after the cmp. The retail move there is a free scheduling choice whose input is still unknown.

Next attack (HYPOTHESIS):
0. Find the post-RA instruction object: hook the allocator that reuses the slot (0x107afd98 caller) or
   log objects passed to the encoder table, then watch one of those.
1. Tuple addresses: change cave W (n17 walker 0x1077fadc, tuple = [esp+0x18]) to log the tuple pointer and
   opcode for RA_FN, giving the pre-RA tuple order with heap addresses (deterministic, also under winedbg).
2. Pick the moved tuple (G_GetPlayerViewOrigin `push A` before/after `fstp [esi+8]`), then winedbg with
   c2dbg.dll (int3 at arm) and `watch` on its link fields: the writer that relinks it after colouring is the
   scheduler. Same method that found the n3 writer (REGALLOC.md round 10).
