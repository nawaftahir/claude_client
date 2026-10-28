# Handoff: CoD2MP_s.exe 1.0 client, 99.664% -> 100%

Read first: dev/CLAUDE.md (laws), ~/.claude/skills/client-recon/SKILL.md, the last ~100 lines of LESSONS.md
(everything dated 10-28 / 10-29 is this push), lab/w200/inl_notes.txt (LTCG inline rule + oracle).

## State (10-29, commit 6da307a, not pushed past 6243a5c)
- Sections same-offset 1694136 / 1699840 = 99.664% from real source. .data and .rsrc are 100%.
- shameter ALL excludes the PE header. The header differs only in the Rich block (0x80-0x107), see below.
  100% sections + Rich block == SHA-identical file (normal release also runs postlink.pl, the documented
  2-byte mixer JE->NOP; account for it separately).
- Build: `bash ~/lv.sh TAG [patch...]` (RAM build in /dev/shm, result in lab/r2w/TAG/sha.txt). Never symlink into lab/r2w.
- Try: lab/w200/<tag>.py with `R = [(old, new[, count])]` exact replacements in ONE file, then
  `cd lab/w200 && bash try.sh <tag> <file under target/src> <tag>.py '<mangled fn>' 0x<retail addr>`.
  Prints ALL and fn diff lines; prints BUILDFAIL on a compile error (22.958% / 390255 = failed build, stale exe).
  Multi-file changes: make each patch with mk.py (or diff -u), then `bash ~/lv.sh TAG p1.patch p2.patch`.
- Land only through `sh lab/gate_w18.sh $PWD/w200/<tag>.patch` (ratchet; reverts on REJECT), then commit.
  If a change already sits in the working tree: `git diff -- target/src > x.patch; git apply -R x.patch`, gate x.patch.

## Remaining bytes (lab/w200/slotonly.py <build>/target/build/release)
Code ~5,200 B in ~150 fns, data ~300 B, header 132 B.
- CG_DrawTracer 621 + ScaleTracer 166: ScaleTracer's LTCG register params (retail: startWidth in ebx only,
  tracerScale/range cached in esi/edi). Codex: noinline on CalcTracerFinalScale gives ScaleTracer 0 lines but
  loses the callee: the callee's register-preservation summary drives the convention (lab/w200/codex100_tracer/).
- CG_AddPlayerWeapon 591: RA (retail weaponNum edi, bViewModel only in [esp+0x10]); 18+ shapes tried (apw*).
- CG_ConfigStringModified 411: one `add` scheduled after `cmp` (lea) in retail; no source lead.
- CG_DrawLagometer 393, CG_CalcCrosshairPosition 392: x87 operand order. lg1.py (snapshot colour before
  scaling) is retail order but 1 B over size.
- Com_ProcessSoundAliasFileLocalization 268, BG_ParseCommands 249: real calls for their .inl stand-ins refuse
  to inline at our counts (per-site r in inl_notes "INL STAND-IN SWEEP 2"); likely compile-order.
- G_RegisterWeapon 213, XAnimCopyAnimTreeNoNotify 128, XAnimGetInfo 56: retail "load eax, test, copy to
  another reg" shape. G_OverrideModel: retail looks like a raw (uncompiled) inline of G_ModelName; the
  G_MODEL_NAME_INLINE macro imitates it. Real G_ModelName makes LTCG pass defaultModelName in eax (64 -> 48 B).
- Dvar_SetFromStringFromSource 169: stack-align frame; call structure equals retail (sa_up/sa_down/sa_cnt.py).
- XModelCalcBasePose 179: x87 sum-term emission order (/Op pragma fn). x87 roles reproduce in a fast
  non-/GL compile: use the s23 mini harness (lab/w200/hvp.py VARS.py) to brute-force term orders before full builds.
- Data: Item_CorrectedTextRect `rect` static must compile one sweep later (needs a two-level vanished callee;
  none found in any reference, do not invent). FX vtable order (Tail Effect Cylinder Cloud OP Particle Light
  Emitter Line in retail): ctors are out-of-line on PC (Mac inline ctors moved code, dead end).
- Header (Rich): C++ /GL objects 265 vs retail 271 (6 TUs missing; com_profilemapload added 10-29). Our 15
  unknown/after_*.cpp placeholders have invented identities. Entries (28,9466,1)/(25,9466,2) swapped (lib order).

## What paid (keep using)
- Q3 / CoD4 (refrences/KisakCOD) source shapes; one check per `if` + `return 1`; `x >= 0 ? 0 : 1` to a bool param.
- A value read into its own local before a call changes register choice (SND_RestoreSoundAlias offset).
- Inline call-site counts: loop exits and enclosing loops (UI_LoadMenus Kisak do/while; CL_MapLoading one-trip
  local-client loop). Oracle tools: lab/w200/lmv.sh, lmsum.py (rebuild fp_t first, see inl_notes).
- Stack slots: Mac 1.3 STABS local lists (claude_client/stabs.py -> mac_stabs.json); dead-store vec copies.
- Verified Linux server source (cod2engine/archive/lnxded-1.0, -O0) as statement-shape oracle: srvcmp.py, srvport.py.
- Mac STABS file list for TU identities; vanished /GL callees from Q3 (Sys_DefaultCDPath fixed cddir).

## Tools added this push (lab/w200)
slotonly.py, framecmp.py (frame-size diffs), addrdelta.py + bsspairs.py (data-address deltas), vtrefs.py
(vtable refs), srvcmp.py, srvport.py, lmv.sh, lmsum.py, reach.py, sa_*.py, emu3.py (sweep order replay).
