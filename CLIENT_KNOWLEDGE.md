# CoD2MP_s.exe 1.0: everything learned (state 10-29, 99.664%)

Companion to HANDOFF_100.md (state + commands). This file is the knowledge: how the compiler decides, what
paid, what did not, and a dossier per remaining function. Read it fully before changing source.
Sources: LESSONS.md (dated lines), ~/.claude/skills/client-recon/SKILL.md, lab/w200/{inl_notes,frame_notes,
x87_notes,ra_notes}.txt, lab/w200/codex100_*/.

## 0. Ground rules that bite
- Real readable C/C++ only. No asm, byte arrays, _emit, invented callees/constants. A probe with a fake
  `if (0) Foo();` proves a mechanism but is never shipped (lab/lg5.patch is the example).
- Score = whole-file same-offset bytes (shameter "ALL"). It excludes the PE header; see section 7.
- A function diff that shrinks while ALL drops usually means the function changed SIZE and shifted everything
  after it (ALL falls to 60-90%). Report it as a direction, never land it alone.
- 22.958% (390255) or similar huge drops with fn=0 = FAILED COMPILE (lv.sh measured a stale exe).
  try.sh now prints BUILDFAIL. Always check `grep 'error C' lab/r2w/TAG/mk.log`.
- Gate (lab/gate_w18.sh) is the only way into main. It ratchets matched functions; "(tie/label)" LOST lines
  with RATCHET OK are fine. A REJECT reverts the patch. Measure whole-file after every landing.
- Never git checkout/restore/stash/reset in claude_client. To gate a working-tree change:
  `git diff -- target/src > x.patch; git apply -R x.patch; sh lab/gate_w18.sh $PWD/x.patch`.
- New files: mk.py edits one file only. Build a multi-file patch with `diff -u` (new file: --label /dev/null).
  New TUs go into target/src/tus.txt (link order; it is sorted by basename within the retail layout).
- Never symlink into lab/r2w (a cleanup once wiped cod2engineold). Anchors are read-only. Disk is tight:
  builds run in RAM (/dev/shm via ~/lv.sh); delete your lab/r2w/<tag> dirs when done.
- Commit identity Nawaf, short messages, no Co-Authored-By. Push only on the owner's "push it".

## 1. The compiler (MSVC 7.1 build 3077, /O2 /GL + link /LTCG): decoded mechanisms

### 1.1 Inline decision per call site (c2 0x1078d1aa via 0x1078cf54)
- cost = size81 - argcost + [site+0x4e] * size81 / 254
  - size81 = callee's COMPILED byte size (callee compiled first); 0 if the callee is not compiled yet ->
    raw-body inline (its own callees then costed separately).
  - argcost = 1 + 5 per constant/address/memory arg, else 2 per arg; +3 if the result is stored to memory.
  - [site+0x4e] = loop weight byte = 1 + maxFlaggedBlocksInOneLoop*254/[0x10882b58]. The loop term looks
    capped near 120 (HYPOTHESIS, from UI_LoadMenus back-solve).
- cost < 14 -> inline.
- Else, with site count c (1e8 = function entry):
  r = (cost*1e8/c + 6*cost)/7 if c <= 1e8, else (cost*1e8/c + 2*cost)/3. Refuse if r >= 71. Veto 0x1078f15a.
- Site count: if-arms halve; a loop header is 10x its entry; code after a loop keeps x5; each loop-exit edge
  passed takes an equal share of the exit flow; `continue` cutting all exits -> 0. Nested sites inside an
  inlined callee take the outer site's count (floor 1e8).
- Consequence: a callee with cost >= 71 can never inline at a site with count <= 1e8 (r >= cost). If retail
  inlines it, retail had a LOOP around it or compiled the caller before the callee (raw inline).
- PAID examples:
  - UI_LoadMenus: our `while(1)` + if/break had 6 exits -> 2nd I_stricmp site count 9.1667, r 71, refused.
    Kisak shape `do token = Com_Parse(&p); while (token && token[0] && token[0] != '}' && I_stricmp(token, "}")
    && (I_stricmp(token, "loadmenu") || Load_Menu(&p, imageTrack)));` = 5 exits -> 9.2, both inline. Match.
  - CL_MapLoading: UI_CloseAll site count 0.375 (r 97). CoD4 one-trip local-client loop
    `for (localClientNum = 0; localClientNum < 1; localClientNum++) { Con_Close(); clients[localClientNum]... }`
    (clients is clientActive_t clients[1]) folds away but multiplies later sites by 5 -> UI_CloseAll r 66,
    Menus_CloseAll 55, Dvar_SetStringFromSource 62 inline. Match (+429). Evidence: CoD4 lineage (SUPPORTED).
- Oracle: lab/w200/lmv.sh FILE SPEC FN (applies a spec in fp_t, rebuilds one obj, relinks under the c2y.dll
  cave, ~17 s) + lmsum.py fp_t/build/ordq.txt FN (one line per site: size, c0/c1 argcost, count, 4e, r).
  fp_t/build/obj and /tmp/orcy were deleted; rebuild per inl_notes.txt (wsites.sh X 1, docker cp c2.dll).

### 1.2 Compile order (sweeps) -> local statics, float pool, vtables, raw inlines
- LTCG compiles in sweeps; each sweep walks TUs in reverse link order and functions in reverse definition
  order; a function compiles only once all its /GL callees are compiled, else it waits for the next sweep.
- Emitted when a function compiles: its local statics (.bss), its float constants (__real@ pool order),
  vtables it references (.rdata COMDATs). So a static's/constant's position = its function's sweep.
- Any call in the IL counts, even if inlined to nothing or dead. A vanished callee changes placement only.
  PAID: Sys_ScanForCD's cddir needed one more level; the original calls Sys_DefaultCDPath() (Q3 win_main,
  returns "") inside the sprintf -> correct placement (Codex, +247 with the layout fixes).
- Item_CorrectedTextRect `rect` must compile in sweep 2 (ours sweep 1). Its only callee Item_Getter_5338c0 is a
  sweep-0 leaf. Needs a two-level vanished chain; none found in Q3, coduomp, CoD1, Mac (Mac line table matches
  our statements exactly), Kisak. Do NOT invent one. Replay tool: lab/w200/emu3.py.
- Explicit `= 0` globals and function-local statics go to an END group of .bss in definition order; other
  uninitialised globals stay in their TU block. ALIGNED(128) must be on the definition AND every extern.
- Raw inline: if the caller compiles before the callee, the callee's raw IL is inlined, producing temp copies
  an optimised inline would not (suspected for G_OverrideModel, section 4).

### 1.3 Stack slot packer (c2 0x10790ad6), fully decoded
- Items sorted: size ascending, then use weight (+0x38) descending, then creation order.
- Params get their own fixed entries first.
- LAST-FIT: 0x10791426 scans entries newest -> oldest; an item joins the first entry where item size <= 2 x
  entry size, the item's bit is not in the entry's interference union, and the entry's members do not
  intersect the item's interference set. Joining ORs the bit, grows the union, adds weight, size = max.
- Offsets: entries sorted by weight*1000/size descending (unstable K&R qsort), allocated downward from the last;
  the densest entry gets the lowest esp offset. 8-aligned entries pad when an odd number of words sits above.
- A light item shares with the most recently created compatible entry, not the heaviest.
- A bool local is a 1-byte item that will not share a 4-byte entry; an int-sized value passed to a bool param
  (e.g. `x >= 0 ? 0 : 1`) gives dword stores + byte reads (UI_Init, +531).
- Dead-store copies keep their slots: retail's unreferenced slots next to playerY were a `vec3_t playerPos;
  VectorCopy(cg->refdef.vieworg, playerPos);` with only [1] used (compass, +222).
- Declaration order does NOT matter; scope (block vs function) and which values are separate variables do.
- Tools: lab/w200/cslot/pk/{sim.py, psim.py, whatif.py, sets.sh}, framecmp.py (frame-size diffs; only the
  compass differed and it is fixed now).

### 1.4 Register allocation
- Priority = use count x region weight x candidates; ties by later last use; try order eax ecx edx esi edi
  ebx ebp. A variable left with one register was beaten by an earlier temp.
- Reading a value into its own local BEFORE a call changes allocation: SND_RestoreSoundAlias
  `offset = MemFile_ReadShort(memFile); return SND_GetAliasWithOffset(name, offset);` (was inline in the
  argument) -> SND_RestoreStreamChannel full match (+1374).
- LTCG custom calling conventions: the number of register params a function gets depends on which registers
  its body leaves free. G_OverrideModel: retail loads the inlined configstring into eax then copies to esi, so
  eax is busy and defaultModelName comes on the stack; with the real G_ModelName ours loads into esi, eax is
  free, LTCG passes defaultModelName in eax and the function shrinks 64 -> 48 B. ScaleTracer: retail gets only
  startWidth in ebx; ours gets both width pointers in esi/edi. Codex showed the callee's register-preservation
  summary (CalcTracerFinalScale) drives it.

### 1.4b LTCG callee register summary (decoded 10-29, lab/w200/conv_notes.txt)
- After a function compiles, c2 stores in its record: +0x21 bit 0x08 = summary valid, +0xc3 = mask of registers it
  never touches (eax 1, ecx 2, edx 4, esi 8, edi 0x10, ebx 0x20, ebp 0x40). Hook 0x107724f6 (compile start).
- A later-compiled caller may keep values in eax/ecx/edx across a call when the callee's mask leaves them free.
- Summary valid only if the function is compiled with /Og AND every callee in its call list already has a valid
  summary. Cleared by: #pragma optimize(g, off) on it (changes its code), a live call to a not-yet-compiled fn.
  NOT cleared by: noinline, static/extern, extern C, dllexport, address taken, definition order, second caller,
  /Oy-, pragmas a/w/p/s/t, inlined helpers.
- ScaleTracer: ours sees CalcTracerFinalScale mask 7e (touches only eax) -> keeps tracerScale/range in ecx/edx, both
  width pointers become esi/edi register params. Retail treats the call as clobbering eax/ecx/edx -> esi/edi hold
  tracerScale/range, startWidth in ebx, finishWidth from the stack in ebp. Retail CalcTracerFinalScale (byte-identical,
  pure leaf, no intrinsics) therefore had NO valid summary; cause still unknown (no vanished call found).
- G_OverrideModel / XAnimGetInfo / XAnimCopyAnimTreeNoNotify / CL_MouseEvent are NOT this mechanism (their callees
  have no summary in ours either): their eax-vs-stack param follows the function's own register allocation.
- Tools: lab/w200/cave_cv.s + patch_cv.py (record dump cave), /home/nawaf/mlw/ora/cv/ (10 s LTCG test bench, v0 = ours,
  v1/v7 = retail convention).

### 1.5 x87 operand roles
- For fadd/fmul with both operands in memory, c2 numbers memory objects; the LOWER number becomes the memory
  operand, the higher gets fld. Scalars/globals: number = first reference order in the function (an assignment
  target counts first). Locals/params/[reg+x] become the memory operand against a global.
- `a*b` vs `b*a`, declaration order, use counts: never matter. Earlier statements DO (retail tests the stored
  copy: `color[3] = alpha; if (color[3] == 0.0f) return;`).
- The pick follows the base-pointer symbol: a named pointer local for a repeatedly used base fixes roles
  (FX_RandomizeRoll `float *v0 = ax[0];`, +8, match).
- One extra cast node from a DIFFERENT type renumbers the whole function's IL (sb_encode, +3713).
- `(double)` cast on a float product in /Op code swaps roles; float overload of atan2 moves fpatan one slot.
- x87 roles reproduce in a fast NON-/GL compile of the TU alone: use it to brute-force before full builds
  (s23 mini harness, lab/w200/hvp.py VARS.py; /home/nawaf/mlw/ora/tiny/run.sh for `cl /O2 /FAs`).

- x87 BLOCK RULE (10-29, lab/w200/x87_rule.txt): a value computed in the SAME basic block as its fmul/fadd is
  consumed (it takes the fld side, the other operand is the memory operand: retail "fsub [a]; fmul [scale]"); a
  named local assigned before any block boundary is a variable kept on the FPU stack (ours "fld [scale];
  fmul st(1) ... fstp st(0)"). A constant-folded switch(3) counts as a boundary. Numbering still decides within one block.
  CG_DrawLagometer: only by-value CL_DrawStretchPic(x, y, w, h, 3, 3, ...) reproduces retail's snapshot site (args
  evaluated right to left, x assigned last in the inlined body's first block), but LTCG then refuses the nested
  CalcScreenPlacement inline (fn 928 -> 903 B). Next: make that nested inline happen (site counts, 1.1).
  CG_CalcViewValues: retail's inlined BG_GetSpeed copy uses memory operands while its standalone copy is CSE'd like
  ours (different IL, cross-TU). CG_CalcCrosshairPosition: evaluation order, not roles. CG_DrawTracer: shifted by one
  extra prologue push (ebx). Fast harness: /home/nawaf/x8/x8h.py SPEC.py (16 variants of a real TU, no /GL, ~3 s).

### 1.6 Stack alignment
- A direct call to a self-aligning function (`and esp,-8`) from a function with LTCG-local callers makes that
  function 8-aligned (frame +8, `push ecx`/`and esp,-8` in callers, lost tail jmps). Retail avoids it when the
  call arrives via LTCG-inlining a real external function; TU-local forceinline/static copies propagate it.
- Dvar_SetFromStringFromSource is aligned in ours, not in retail, although its whole call structure (562 fns,
  6 levels, every align flag) equals retail: points at remaining hand-inlined code feeding site counts.

### 1.7 Scheduling / block order
- Tail-merge placement of identical return blocks follows source structure: one check per `if` with a
  separate final `if` + `return 1` (LAN_ReadServerCache match), Q3 return shapes (CL_CDKeyValidate match).
- Single-instruction swaps (an independent load before/after a store) mostly did not move with statement
  order (Item_ListBox_MouseEnter, compass's last 3 swaps, CG_ConfigStringModified's lea).

## 2. Evidence sources (in order of value)
1. Retail binary itself (lab/w200/sa_dis.py prints retail disasm with names).
2. Verified Linux server source cod2engine/archive/lnxded-1.0/src, built -O0 and SHA-identical: its statement
   shape is the original. lab/w200/srvcmp.py FN... (body diff), srvport.py TAG FN (spec with the server body).
   Several client functions are already token-identical (XAnimGetInfo, XAnimCopyAnimTreeNoNotify,
   G_RegisterWeapon, Dvar_SetFromStringFromSource, SV_GetConfigstringConst): their diffs are compiler effects.
   The server body can still be bigger in our build because of inlining (EvalPrimitiveExpression).
3. Mac 1.3 client STABS (claude_client/stabs.py -> mac_stabs.json): real function names per source file, local
   variable names and scopes, types, line tables. Gave the compass locals, buildInfo[128], LegacyHacks,
   com_profilemapload. Caveat: Mac inlines/defines some things differently (FX ctors inline on Mac, out-of-line
   on PC: PC code layout proves it).
4. Quake 3 (refrences/Quake-III-Arena) and CoD4 KisakCOD (refrences/KisakCOD): original idioms and loop shapes.
5. coduomp, opencod1, opencod2, CoD2rev: structure hints only.
- NOT useful: the 1.2/1.3 Windows clients. Same toolchain (Rich build 3077), no debug info, and the hard
  functions are byte-identical to 1.0. CL_MapLoading source changed in 1.2. PunkBuster/libwww are 1.2 additions.

## 3. Techniques that paid (with the function they fixed)
| Technique | Example | Gain |
|---|---|---|
| int ?: passed to a bool param instead of a bool local | UI_Init `Dvar_SetBoolByName("ui_mousePitch", Dvar_GetFloat("m_pitch") >= 0.0f ? 0 : 1)` | +531, match |
| One check per if + final `if (...) return 0; return 1;` | LAN_ReadServerCache | +178, match |
| Q3 return shape | CL_CDKeyValidate `if (checksum && !I_strnicmp(...)) return 1; if (!checksum) return 1; return 0;` | +16, match |
| Real call instead of .inl stand-in where the count allows | UI_LoadMenus first I_stricmp | +53 |
| Loop exit shape (Kisak) | UI_LoadMenus | +19, match |
| One-trip local-client loop + Q3 statement order | CL_MapLoading | +429, match |
| Value into its own local before the call | SND_RestoreSoundAlias offset (+ per-alias type check) | +1374, match |
| Values shared at function scope / reuse a variable | compass: all floats at function scope, angle reuses yaw | +382, +822 |
| Dead-store vec copy | compass playerPos | +222 |
| Mac local names/scope | compass baseColorModdedByComapassFadeOut at function scope | +11 |
| Read the global directly instead of a param | ScaleTracer reads cg->refdef.vieworg | +132 (with vertex order) |
| Statement order of constants vs math | CG_DrawTracer vertex constants before each VectorMA | (part of +132) |
| VectorCopy + `+=` instead of `a + b` | CG_AddPlayerWeapon lighting origin | +2 |
| Vanished callee from Q3 | Sys_ScanForCD Sys_DefaultCDPath() | (Codex +247 set) |
| Mac sizes/types for data | buildInfo[128], LegacyHacks type, aligned msgHuff | .data 100% |
| Own TU for a Mac file | com_profilemapload.cpp (0x42bc80 stub) | Rich +1 |

## 4. Remaining functions: dossier (bytes = differing bytes at 99.664%)
- CG_DrawTracer 621 / ScaleTracer 166 (oc/cgame_mp/cg_weapons.cpp): AddMovingTracer is 2 lines now. Left:
  ScaleTracer convention (retail: stack args start, finish, finishWidth (loaded into ebp at entry); startWidth
  in ebx; esi/edi cache tracerScale and range across the two CalcTracerFinalScale calls), DrawTracer's two
  dot products (retail `fmul [A]; fxch; fmul [A]` in place for line[1], `fld st(2); fmul [A]` for line[0]).
  Tried: width param swap/order, locals order, dot term order, `finishWidth = startWidth`, line[1] first
  (+6 noise, worse fn), DotProduct helper (tr2 was a failed build; redone: no effect). Next: the callee's
  register summary (noinline CalcTracerFinalScale gives ScaleTracer 0 lines but loses the callee).
- CG_AddPlayerWeapon 591: retail cent ebx, ps ebp, parent esi, weaponNum edi (reloaded at the join after the
  gun-block memset), bViewModel only in [esp+0x10]. Tried apw1-18, playerState alias (opencod2), merged
  if/else (size change). Open: WeaponFlash convention (weaponNum in eax), gun block as an inlined helper.
- CG_ConfigStringModified 411: retail schedules the configstring base `add` after `cmp esi,7` (as lea). The
  standalone CL_GetConfigString matches with add. Tried early num==7 return (-10), bool temp, many body forms.
- CG_DrawLagometer 393: lg1.py (snapshot colour before `v *= vscale`) is retail order, 1 B over size (929 vs
  928). Remaining: x87 in the inlined placement of the snapshot loop (retail `fmul [scaleVirtualToReal0]`
  consumes x; ours `fld scale; fmul st(1)` keeps x). Real CalcScreenPlacement calls give the same code.
  Fixing both snapshot sites would save ~8 B. Use the x87 rule (1.5) and the fast non-/GL harness.
- CG_CalcCrosshairPosition 392: retail stores the tan argument, computes the dot on the FPU stack, then calls
  tan. Our forms: dot local spills it (ch3/ch9); inline dot calls tan first (ch4/6/8). ch1 (inline dots) is
  closest (62 -> 53 lines) but changes size.
- Com_ProcessSoundAliasFileLocalization 268: RA (retail keeps f in esi over the parse loop). Real
  Com_EndParseSession inlines here once noinline goes, but then 3 more sites inline that retail calls
  (Com_LoadVolumeModGroups end, Com_ParseVolumeFalloffCurve x2): compile-order question (cycle via Com_Error),
  unproven.
- BG_ParseCommands 249: server body = ours apart from the .inl stand-ins. The strnicmp stand-in had s0/s1
  swapped; s0 = token, s1 = "sound" leaves only the loop form (79 -> 15 lines) but ALL -54 (size). Real calls
  refused: Com_ParseOnLine (count 0.708, needs > 1.108), nested I_strnicmp (6.64, needs > 9.18); the outer loop
  entry count is only 2.5. Find what raises it (server statement order is fixed).
- G_RegisterWeapon 213 / XAnimCopyAnimTreeNoNotify 128 / XAnimGetInfo 56: all show retail's "load into eax,
  test, copy to another register" shape. G_OverrideModel uses a G_MODEL_NAME_INLINE macro stand-in that
  reproduces retail's standalone G_OverrideModel; the real G_ModelName fixes G_RegisterWeapon to 7 lines but
  shrinks G_OverrideModel. Theory: retail inlined G_ModelName raw (compile order). Source text of all three
  equals the server. One fix probably covers ~400 B.
- XModelCalcBasePose 179 (xanim/xmodel_load.cpp, #pragma optimize("p") /Op): x87 add-chain emission order.
  A 2-term + chain emits right-first; reordering terms did not map simply (xb1/xb2 worse). Brute-force term
  orders with the fast non-/GL harness.
- Dvar_SetFromStringFromSource 169: alignment (1.6).
- Sys_IsLANAddress 153: retail keeps separate `mov eax,1; ret` blocks after each range test; ours shares a
  flag-based tail. One-if-per-return forms give identical bytes.
- CG_InitXAnimTrees 152: indexed loops `cg->bgs.clientinfo[i].pXAnimTree = ...` give retail's end-pointer loops
  (xt1, 41 lines) but retail also hoists &anims->size into a stack slot (push ecx frame); size differs.
- CG_DrawCrosshairNames 114, SND_RawSamples 106, CL_MouseEvent 53: register pairs only.
  CL_MouseEvent: retail dy arrives in eax (LTCG convention), ours in edx.
- FX_CalcOriginAndAxis 86 (slot-only, +4 shift of the GetVal temps), Item_CorrectedTextRect 54 (data), small
  tail of ~120 functions with 1-30 B each (scheduling, register pairs, data addresses).
- speex _speex_bits_pack 29: upstream speex 1.1.9 text (retail string "speex-1.1.9"); only the tail-merge
  choice between the two warning paths differs.

## 5. Data and layout
- .data and .rsrc are 100%. .rdata 99.74%, mostly float-pool order (compile order) and FX vtables.
- Data-address diffs (lab/w200/addrdelta.py, bsspairs.py BUILD SYM): rect static (-0x18 group x42 refs and
  +0x2ad0), FX vtables (+0x2c, +0x58, -0x58, -0x108), cgArray/cls small deltas, Com_ErrorCleanup -0x400 pair.
- FX vtable order retail: Tail Effect Cylinder Cloud OrientedParticle Particle Light Emitter Line (Flash
  elsewhere); ours Effect Cloud Tail OP Light Emitter Line Particle Cylinder. Referenced by each ctor, FX_Add<X>
  and FX_Restore (lab/w200/vtrefs.py). Ctors are out-of-line on PC (code positions prove it).

## 6. Stand-ins still in the tree (must become real calls before release)
- ~38 `.inl` includes (grep '#include ".*\.inl"' target/src): q_parse_endsession (many), screen_placement
  (cg_draw x4, drawtools, console, scrn), q_shared_strlwr, q_parse_online/skipline, scr_* variable helpers,
  cl_keyevent, cl_init_server_info, xanim_settime, quat_multiply, q_shared_strnicmp.
- UI_Init's one-trip `for (netGameTypeIdx = 0; netGameTypeIdx < 1; ...)` around Dvar_RegisterInt
  (estimator stand-in; may be a real local-client loop like CL_MapLoading's: check with the oracle).
- EvalPrimitiveExpression `goto evalFloat` shared tail; G_MODEL_NAME_INLINE macro; XModelCalcBasePose pragma.
- Removed this push: q_shared_stricmp.inl, ui_closeall.inl, Dvar_RegisterVariant __forceinline.
- Rule: replace one site at a time and measure; replacing all 24 Com_EndParseSession stand-ins at once grew
  .text by 96 B (lab/w200/codex100_root/endsession_calls.patch).

## 7. PE header (Rich block, 132 B)
- Only 0x80-0x107 differ. Decode: lab one-liner in LESSONS (XOR key at 'Rich'+4).
- C++ /GL objects (id 100, build 3077): retail 271, ours 265. 6 TUs missing. Already present as empty TUs:
  mem_track, snd_utils, scr_evaluate, scr_debugger, fxprofile, legacyhacks. Ruled out: PunkBuster, libwww,
  fft/com_convexhull (renderer DLL), Kisak speex extras. Our 15 unknown/after_*.cpp placeholders have invented
  identities: if their storage belongs to neighbour TUs, more real TUs are missing. Candidates need per-TU
  evidence (stub boundaries in retail + Mac file names sorted into link order).
- Entries (28,9466,1) and (25,9466,2) are swapped: link input order of old prebuilt lib objects (reversing the
  Miles archive order did not fix it).

## 8. Tools (lab/w200 unless noted)
try.sh, mk.py, ~/lv.sh, ../m3_fx.sh (fn diff), slotonly.py BUILD [N] (ranked diffs, slot-only class),
framecmp.py, addrdelta.py, bsspairs.py, vtrefs.py, srvcmp.py, srvport.py, lmv.sh + lmsum.py (inline oracle),
reach.py, emu3.py (sweep replay), sa_dis.py, sa_fsz.py (fn sizes between maps), sa_up/sa_down/sa_cnt.py
(call graph, align flags), cslot/pk/* (packer sim), hvp.py (fast mini harness), claude_client/stabs.py,
fdiff.py, shameter.py.
