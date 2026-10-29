# CoD2MP_s.exe 1.0: everything learned (state 10-29, 99.679%)

Companion to HANDOFF_100.md (state + commands). This file is the knowledge: how the compiler decides, what
paid, what did not, and a dossier per remaining function. Read it fully before changing source.
Audited 10-29: superseded claims removed or marked WRONG. When this file and LESSONS.md disagree, this file wins
(LESSONS.md is the dated log and keeps old hypotheses).
Sources: LESSONS.md, ~/.claude/skills/client-recon/SKILL.md, lab/w200/{inl_notes,frame_notes,x87_notes,x87_rule,
conv_notes,summary_notes,ra_notes}.txt, lab/w200/codex100_*/.

## 0. Ground rules that bite
- LAW 12 (dev/CLAUDE.md): only VERIFIED findings go into this file, each with evidence and a tag (PINNED /
  SUPPORTED / HYPOTHESIS). Correct or delete disproved claims in place. Before experimenting, check the tried
  lists in section 4 and skip what was already tried; agent briefs must name them.
- Real readable C/C++ only. No asm, byte arrays, _emit, invented callees/constants. A probe with a fake
  `if (0) Foo();` proves a mechanism but is never shipped (lab/lg5.patch is the example).
- Score = whole-file same-offset bytes (shameter "ALL"). It excludes the PE header; see section 7.
- A function diff that shrinks while ALL drops usually means the function changed SIZE and shifted everything
  after it (ALL falls to 60-90%). Report it as a direction, never land it alone.
- 22.958% (390255) or similar huge drops with fn=0 = FAILED COMPILE (lv.sh measured a stale exe).
  try.sh prints BUILDFAIL. Always check `grep 'error C' lab/r2w/TAG/mk.log`.
- try.sh exits 1 when there is NO build failure (its last grep finds nothing); read its ALL line, not the code.
- Gate (lab/gate_w18.sh) is the only way into main. It ratchets matched functions; "(tie/label)" LOST lines
  with RATCHET OK are fine. A REJECT reverts the patch. Measure whole-file after every landing.
- Never git checkout/restore/stash/reset in claude_client. To gate a working-tree change:
  `git diff -- target/src > x.patch; git apply -R x.patch; sh lab/gate_w18.sh $PWD/x.patch`.
- New files: mk.py edits one file only. Build a multi-file patch with `diff -u` (new file: --label /dev/null).
  New TUs go into target/src/tus.txt (link order; sorted by basename within the retail layout).
- Shell quoting: backticks and `\x15` inside `bash -lc "..."` heredocs get executed/eaten. Write spec files and
  notes with a file-writing tool, or use quoted heredocs only.
- Never symlink into lab/r2w (a cleanup once wiped cod2engineold). Anchors are read-only. Builds run in RAM
  (/dev/shm via ~/lv.sh); delete your lab/r2w/<tag> dirs when done.
- Commit identity Nawaf, short messages, no Co-Authored-By. Push only on the owner's "push it".

## 1. The compiler (MSVC 7.1 build 3077, /O2 /GL + link /LTCG): decoded mechanisms

### 1.1 Inline decision per call site (c2 0x1078d1aa via 0x1078cf54)
Two paths, chosen by whether the callee is already compiled:
- COST-MODEL path (callee compiled; normal case): the oracle prints a W line for the site.
  - cost = size81 - argcost + [site+0x4e] * size81 / 254 - constant-arg discount
    - size81 = callee's compiled byte size.
    - argcost = 1 + 5 per constant/address/memory arg, else 2 per arg; +3 if the result is stored to memory.
    - [site+0x4e] = loop weight byte = 1 + maxFlaggedBlocksInOneLoop*254/[0x10882b58]; loop term looks capped
      near 120 (HYPOTHESIS).
    - constant-arg discount (FUN_10797769, c1 in the oracle V line): read from a CALLEE-side branch summary (see
      1.4c); e.g. CalcScreenPlacement with constant aligns: c1 431-435, cost drops from ~437 to ~6.
  - cost < 14 -> inline. Else with site count c (1e8 = function entry): r = (cost*1e8/c + 6*cost)/7 if
    c <= 1e8, else (cost*1e8/c + 2*cost)/3. Refuse if r >= 71. Veto 0x1078f15a. No per-caller budget on this
    path (only a penalty above 9000).
- BUDGET path (callee NOT compiled yet; no W line, size81 = 0; 1153 of 1175 W-less sites): the caller has a
  running budget, 1000 minus what it already inlined (V line p4). Refuse if [0x10882b5c] > 35000, or if
  p4 < callee IL size [callee+0x6d] and that IL size > 40. Otherwise inline the raw IL.
  WRONG (old claim): "uncompiled callee = always raw-body inline".
- Site count: if-arms halve; a loop header is 10x its entry; code after a loop keeps x5; each loop-exit edge
  passed takes an equal share of the exit flow; `continue` cutting all exits -> 0. Nested sites inside an
  inlined callee take the outer site's count (floor 1e8). Counts depend on the function's state at decision time
  (a real Com_Parse call at a loop top gets the pre-loop count; the macro-expanded body gets the loop-header count).
- A callee with cost >= 71 can never inline at a site with count <= 1e8 (r >= cost): if retail inlines it, look
  for a loop around the site (or the budget path).
- PAID: UI_LoadMenus (Kisak do/while, 5 exits -> both I_stricmp inline, match); CL_MapLoading (CoD4 one-trip
  local-client loop `for (localClientNum = 0; localClientNum < 1; localClientNum++) { Con_Close(); clients[...] }`
  multiplies later sites x5 -> UI_CloseAll/Menus_CloseAll/Dvar_SetStringFromSource inline, match +429).
- Oracle: lab/w200/lmv.sh FILE SPEC FN (spec into fp_t, rebuild one obj, relink under /tmp/orcy/vc c2y.dll,
  ~15 s) + lmsum.py. /tmp/orcy is rebuilt 10-29 (rebuild recipe in inl_notes.txt if /tmp was cleared).

### 1.2 Compile order (sweeps) -> local statics, float pool, vtables
- LTCG compiles in sweeps; each sweep walks TUs in reverse link order and functions in reverse definition
  order; a function compiles only once all its /GL callees are compiled, else it waits for the next sweep.
  A leaf that is not in a call cycle always compiles before its callers.
- Emitted when a function compiles: its local statics (.bss), its float constants (__real@ pool order),
  string literals it references first, vtables it references (.rdata COMDATs).
- Any call in the IL counts, even if inlined to nothing or dead. PAID: Sys_ScanForCD's cddir: the original calls
  Sys_DefaultCDPath() (Q3 win_main, returns "") -> correct placement.
- Item_CorrectedTextRect `rect` must compile in sweep 2 (ours sweep 1); needs a two-level vanished chain; none
  found in Q3, coduomp, CoD1, Mac (Mac line table matches our statements), Kisak. Do NOT invent one (emu3.py).
- Explicit `= 0` globals and function-local statics go to an END group of .bss in definition order; other
  uninitialised globals stay in their TU block. ALIGNED(128) must be on the definition AND every extern.
- Forward class declarations change vtable emission order (Argon: `class Light; class Particle; ...` at the top of
  FxPrimitives.cpp fixed the FX vtable order).
- WRONG (old claim): "G_OverrideModel = raw inline of an uncompiled G_ModelName". Definition order has no effect
  (aw1/aw2); see 1.4.

### 1.3 Stack slot packer (c2 0x10790ad6), fully decoded
- Items sorted: size ascending, then use weight (+0x38) descending, then creation order. Params get fixed entries.
- LAST-FIT: 0x10791426 scans entries newest -> oldest; an item joins the first entry where item size <= 2 x
  entry size, the item's bit is not in the entry's interference union, and the entry's members do not
  intersect the item's interference set.
- Offsets: entries sorted by weight*1000/size descending (unstable qsort), allocated downward; densest entry
  gets the lowest esp offset. 8-aligned entries pad when an odd number of words sits above.
- A bool local is a 1-byte item that will not share a 4-byte entry; an int-sized value passed to a bool param
  (e.g. `x >= 0 ? 0 : 1`) gives dword stores + byte reads (UI_Init, +531).
- Dead-store copies keep their slots (compass playerPos vec3_t, +222). Declaration order does NOT matter;
  scope (block vs function) and which values are separate variables do (compass angle reuses yaw, +822).
- Tools: lab/w200/cslot/pk/{sim,psim,whatif}.py, sets.sh, framecmp.py (no frame-size mismatch left).

### 1.4 Register allocation and LTCG calling conventions
- Priority = use count x region weight x candidates; ties by later last use; try order eax ecx edx esi edi
  ebx ebp. A variable left with one register was beaten by an earlier temp.
- Reading a value into its own local BEFORE a call changes allocation (SND_RestoreSoundAlias offset, +1374).
- A function's own register allocation decides whether a param arrives in a register: G_OverrideModel (retail
  loads the configstring into eax then copies to esi, so eax is busy and defaultModelName comes on the stack;
  ours with the real G_ModelName/`if (!cs[i])` shape loads into esi and passes defaultModelName in eax, 64 -> 47 B;
  only the `cs ? cs : ""` macro stand-in keeps 64 B). XAnimGetInfo, XAnimCopyAnimTreeNoNotify, CL_MouseEvent: same
  class (own RA), not the callee summary.

### 1.4b LTCG callee REGISTER summary (decoded 10-29, lab/w200/conv_notes.txt)
- After a function compiles, its record holds: +0x21 bit 0x08 = summary valid, +0xc3 = mask of registers it
  never touches (eax 1, ecx 2, edx 4, esi 8, edi 0x10, ebx 0x20, ebp 0x40). Hook 0x107724f6.
- A later-compiled caller may keep values in eax/ecx/edx across a call when the callee's mask leaves them free.
- Valid only if compiled with /Og AND every callee already has a valid summary. Cleared by #pragma optimize("g",
  off) (changes code) or a live call to a not-yet-compiled function. NOT cleared by noinline, static/extern,
  extern "C", dllexport, address taken, definition order, second caller, /Oy-, pragmas a/w/p/s/t, inlined helpers.
- ScaleTracer: ours sees CalcTracerFinalScale mask 7e (touches only eax) -> tracerScale/range stay in ecx/edx,
  both width pointers become esi/edi register params. Retail behaves as if CalcTracerFinalScale has NO valid
  summary (esi/edi hold tracerScale/range, startWidth in ebx, finishWidth from the stack in ebp), although the
  callee is byte-identical, a pure leaf, no intrinsics, not address-taken.

### 1.4c LTCG callee CONSTANT-ARG summary + the key lead (10-29)
- The constant-arg discount (FUN_10797769, decompile lab/w200/ghx/discount_10797769.c) walks a CALLEE-side branch
  summary built when the callee compiles (per node: param index, flag bit 1 = branch depends on the param,
  weights /0xfffe scaled by size); FUN_1079c4f0 checks whether the constant arg eliminates the branch.
- Retail CalcScreenPlacement (0x449ee0, switch jump tables like ours, not address-taken) behaves as if it has NO
  such summary: retail calls it at all 48 sites (our 33 variable-align + 15 constant-align) and inlines it only via
  the budget path (CG_DrawLagometer through CL_DrawStretchPic; CG_DrawDevString, SCR_DrawDemoRecording,
  Con_DrawSay, which are .inl stand-ins in our tree).
- KEY LEAD: CalcTracerFinalScale (no register summary) and CalcScreenPlacement (no branch summary) are both leaves
  whose per-function LTCG summaries are missing in retail although their code equals ours. Find what makes c2
  skip recording summaries for a function (TU property, flag, second pass). Notes: lab/w200/summary_notes.txt.
  Fixing it likely unlocks the tracer pair (~790 B), CG_DrawLagometer (~390 B), the CalcScreenPlacement noinline
  stand-in and maybe the Com_EndParseSession stand-ins.

### 1.5 x87 operand roles
- Commutative op with both operands in memory: c2 numbers memory objects; the LOWER number becomes the memory
  operand, the higher gets fld. Numbering = first reference order in the function (assignment target first).
  Locals/params/[reg+x] become the memory operand against a global.
- BLOCK RULE (lab/w200/x87_rule.txt): a value computed in the SAME basic block as its fmul/fadd is consumed (takes
  the fld side; the other operand is the memory operand, retail "fsub [a]; fmul [scale]"); a named local assigned
  before any block boundary is a variable kept on the FPU stack (ours "fld [scale]; fmul st(1) ... fstp st(0)").
  A constant-folded switch counts as a boundary. Numbering still decides within one block.
- `a*b` vs `b*a`, declaration order, use counts: never matter. Earlier statements DO.
- Base-pointer symbol identity matters (FX_RandomizeRoll named pointer local, +8). One cast node from a DIFFERENT
  type renumbers the whole function (sb_encode, +3713). `(double)` cast in /Op code swaps roles.
- Fast harnesses (no /GL, seconds): /home/nawaf/x8/x8h.py SPEC.py (16 variants of a real TU, private src copy in
  /home/nawaf/x8/src: refresh from main), s23 mini harness lab/w200/hvp.py, /home/nawaf/mlw/ora/tiny/run.sh.

### 1.6 Stack alignment
- A direct call to a self-aligning function (`and esp,-8`) from a function with LTCG-local callers makes that
  function 8-aligned. Retail avoids it when the call arrives via LTCG-inlining a real external function;
  TU-local forceinline/static copies propagate it.
- Dvar_SetFromStringFromSource is aligned in ours, not retail, although its call structure equals retail.

### 1.7 Scheduling / block order / data-address pairs
- Tail-merge placement follows source structure (LAN_ReadServerCache, CL_CDKeyValidate matches).
- Data-address +8/-8 PAIRS inside one function (bsspairs.py BUILD SYM) = statement/store order, not struct
  layout. PAID: CG_Respawn shared VectorClear (+16), CG_ScreenFade natural copy order (+6), CG_PulseLowHealthOverlay
  operand order (+2), CG_DrawActive offset angles first (+6), CG_PlayLoopedFx cull from predictedPlayerState.origin
  (+9). A pair that does not move with spelling is x87 numbering (CG_OffsetFirstPersonView, CG_CalcViewValues).
- Single-instruction load/store swaps mostly do not move with statement order (Item_ListBox_MouseEnter,
  compass's last 3 swaps, CG_ConfigStringModified's lea, ConDrawInput_DetailedCmdMatch).

## 2. Evidence sources (in order of value)
1. Retail binary itself (lab/w200/sa_dis.py).
2. Verified Linux server source cod2engine/archive/lnxded-1.0/src (-O0, SHA-identical): original statement shape.
   srvcmp.py FN, srvport.py TAG FN. Token-identical already: XAnimGetInfo, XAnimCopyAnimTreeNoNotify,
   G_RegisterWeapon, Dvar_SetFromStringFromSource, SV_GetConfigstringConst, XModelTraceLine, Com_EndParseSession.
3. Mac 1.3 client STABS (stabs.py -> mac_stabs.json): names per file, local names/scopes, types, line tables.
   Caveat: Mac inlines/defines some things differently (FX ctors inline on Mac, out-of-line on PC).
4. Quake 3 (refrences/Quake-III-Arena) and CoD4 KisakCOD (refrences/KisakCOD): idioms, loop shapes.
5. coduomp, opencod1, opencod2, CoD2rev: structure hints only.
- 1.2/1.3 Windows clients: same toolchain, no debug info, hard 1.0 functions byte-identical -> no help for 1.0.

## 3. Techniques that paid
| Technique | Example | Gain |
|---|---|---|
| int ?: passed to a bool param instead of a bool local | UI_Init `Dvar_SetBoolByName("ui_mousePitch", Dvar_GetFloat("m_pitch") >= 0.0f ? 0 : 1)` | +531, match |
| One check per if + final `if (...) return 0; return 1;` | LAN_ReadServerCache | +178, match |
| Q3 return shape | CL_CDKeyValidate | +16, match |
| Real call instead of .inl stand-in where counts allow | UI_LoadMenus | +53 / +19, match |
| One-trip local-client loop + Q3 statement order | CL_MapLoading | +429, match |
| Value into its own local before the call | SND_RestoreSoundAlias offset | +1374, match |
| Function-scope shared floats / reuse a variable | compass | +382, +822 |
| Dead-store vec copy | compass playerPos | +222 |
| Read the global directly instead of a param | ScaleTracer reads cg->refdef.vieworg | +132 (with vertex order) |
| Shared helper instead of a local macro | CG_Respawn VectorClear | +16 |
| Field read the retail address pins | CG_PlayLoopedFx predictedPlayerState.origin | +9, match |
| Statement / operand order from data-address pairs | CG_ScreenFade, CG_DrawActive, CG_PulseLowHealthOverlay | +14 |
| Forward class declarations | FX vtable order (Argon) | part of +204 |
| Vanished callee from Q3 | Sys_ScanForCD Sys_DefaultCDPath() | Codex +247 set |
| Mac sizes/types for data | buildInfo[128], LegacyHacks type, aligned msgHuff | .data 100% |
| Own TU for a Mac file | com_profilemapload.cpp | Rich +1 |

## 4. Remaining functions: dossier (differing bytes at 99.679%)
- CG_DrawTracer 621 / ScaleTracer 166: retail CG_DrawTracer starts 0x4d1480 (not 0x4d1470); retail ScaleTracer is
  239 B vs ours 222 B. ScaleTracer convention = 1.4b/1.4c key lead. DrawTracer dot products (retail in-place
  `fmul [A]; fxch; fmul [A]` for line[1], `fld st(2); fmul [A]` for line[0]); ~20 harness forms tried, only
  inline subtractions give the in-place style with the wrong term order; DrawTracer also has one extra prologue
  push (ebx). Tried (no gain): tz1-6, tr*, tv*.
- CG_AddPlayerWeapon 591: own RA (retail cent ebx, ps ebp, parent esi, weaponNum edi reloaded at the join after
  the gun-block memset, bViewModel only in [esp+0x10]). Tried apw1-18, wp*, ps1/ps2 (opencod2 playerState alias).
- CG_ConfigStringModified 411: retail schedules the configstring base `add` after `cmp esi,7` (lea). No
  CG_ConfigString wrapper exists in the tree (Q3 has one; not added without evidence).
- CG_DrawLagometer 393: lg1.py = retail statement order. CalcScreenPlacement is a __declspec(noinline) stand-in;
  without it (lgo_ni.py) + lg1 + by-value CL_DrawStretchPic in both snapshot sites (x8b) the function has 6 diff
  lines, but 15 constant-align sites elsewhere inline that retail calls (ALL 62.6%). Blocked on 1.4c.
- CG_CalcCrosshairPosition 392: evaluation order (retail stores the tan argument, keeps the dot on the FPU stack,
  then calls tan). ch1 (inline dots) is closest but changes size.
- Com_ProcessSoundAliasFileLocalization 268 and the Com_EndParseSession stand-ins: with all 24 stand-ins real and
  noinline removed, every ES site outside com_sndalias_load_obj.cpp inlines as retail; wrong only in
  Com_ParseVolumeFalloffCurve, Com_LoadVolumeModGroups, Com_LoadSoundAliasFile (es_all.sh, es_sites.py).
  The extra .rdata pointer com_endParseSessionUnderflow (shifts 23 refs) exists only for the stand-ins.
- BG_ParseCommands 249: server body = ours apart from the .inl stand-ins; strnicmp stand-in s0/s1 swapped gives
  15 lines but ALL -54; real calls refused at our counts.
- G_RegisterWeapon 213 / XAnimCopyAnimTreeNoNotify 128 / XAnimGetInfo 56: own RA (1.4); source equals the server.
- XModelCalcBasePose 179: /Op add-chain emission order; brute-force with the fast harness.
- Dvar_SetFromStringFromSource 169: alignment (1.6).
- Sys_IsLANAddress 153: retail keeps separate `mov eax,1; ret` blocks; ours shares a flag tail.
- CG_InitXAnimTrees 152: indexed loops give retail's end-pointer loops (xt1) but retail also hoists &anims->size.
- CG_DrawCrosshairNames 114, SND_RawSamples 106, CL_MouseEvent 53: register pairs.
- FloatSwap 14: retail re-reads f from memory with logical shifts; 5 shift forms all worse (fs1-fs5).
- FX_CalcOriginAndAxis 86 (slot +4), Item_CorrectedTextRect 54 (data, 1.2), ~100 small functions 1-30 B.
- speex _speex_bits_pack 29: upstream 1.1.9 text; tail-merge choice only.

## 5. Data and layout
- .data and .rsrc 100%. .rdata 99.82%: float-pool order (compile order) and the com_endParseSessionUnderflow
  pointer (+4 group, 23 refs; removing it needs the stand-ins gone: writing the literal at each site emits the
  string early, .rdata 92.55%).
- FX vtable order: FIXED (forward class declarations).
- Data-address diffs left (addrdelta.py, bsspairs.py): rect static (-0x18 x42 and +0x2ad0), the +4 group,
  Com_ErrorCleanup statics vs Com_GetSubtitleStringEdReference buffer (compile order via the stand-ins),
  three cgArray x87 pairs.

## 6. Stand-ins still in the tree (must become real calls/code before release)
- ~36 `.inl` includes: q_parse_endsession (24 sites), screen_placement (cg_draw x4, drawtools, console, scrn),
  q_shared_strlwr, q_parse_online/skipline, scr_* variable helpers, cl_keyevent, cl_init_server_info,
  xanim_settime, quat_multiply, q_shared_strnicmp; COM_PARSE_INLINE macro (7).
- __declspec(noinline) stand-ins: CalcScreenPlacement, Com_EndParseSession, CalcTracerFinalScale.
- UI_Init's one-trip `for (netGameTypeIdx ...)` around Dvar_RegisterInt; EvalPrimitiveExpression `goto evalFloat`;
  G_MODEL_NAME_INLINE macro; XModelCalcBasePose pragma; CG_Respawn offset macros (CG_INT/CG_FLOAT).
- Removed: q_shared_stricmp.inl, ui_closeall.inl, Dvar_RegisterVariant __forceinline.

## 7. PE header (Rich block, 132 B)
- Only 0x80-0x107 differ (XOR key at 'Rich'+4).
- C++ /GL objects (id 100, build 3077): retail 271, ours 265 (com_profilemapload added). Already present as empty
  TUs: mem_track, snd_utils, scr_evaluate, scr_debugger, fxprofile, legacyhacks. Ruled out: PunkBuster, libwww
  (1.2+ additions), fft/com_convexhull (renderer DLL), Kisak speex extras. 15 unknown/after_*.cpp placeholders
  have invented identities.
- Entries (28,9466,1) and (25,9466,2) swapped: link input order of old prebuilt lib objects.

## 8. Tools (lab/w200 unless noted)
try.sh, mk.py, ~/lv.sh, ../m3_fx.sh, slotonly.py, framecmp.py, addrdelta.py, bsspairs.py, vtrefs.py, srvcmp.py,
srvport.py, lmv.sh + lmsum.py (inline oracle), es_all.sh, es_sites.py, lgo_sp.sh, lgo_run.sh, lgo_try.sh, rf_*.py,
reach.py, emu3.py, sa_dis.py, sa_fsz.py, sa_up/sa_down/sa_cnt.py, cslot/pk/*, hvp.py, cave_cv.s + patch_cv.py,
ghx/ (Ghidra project + Dec.java), /home/nawaf/x8/x8h.py, /home/nawaf/mlw/ora/cv/ (LTCG convention bench),
claude_client/stabs.py, fdiff.py, shameter.py.
