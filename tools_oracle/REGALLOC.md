# c2.dll register-allocation probe (10-18, first pass)

Status: the logging path works (one LTCG link, ~14 s, filtered to one fn). The allocator decision
point is NOT found yet; the dump shows color.c candidate nodes when they are freed. The field meanings
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
