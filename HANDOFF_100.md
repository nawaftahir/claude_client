# Handoff: CoD2MP_s.exe 1.0 client, 99.650% -> 100%

Read first: dev/CLAUDE.md (laws), ~/.claude/skills/client-recon/SKILL.md, the last ~80 lines of LESSONS.md.

## State (10-28, commit c3703f6)
- Whole-file same-offset 1693888 / 1699840 = 99.650% from real source. 100% same-offset == SHA-identical file.
- Build: `bash ~/lv.sh TAG [patch...]` (RAM build, result in lab/r2w/TAG/sha.txt). Never symlink into lab/r2w.
- Try a source change: write lab/w200/<tag>.py with `R = [(old, new)]`, then
  `cd lab/w200 && bash try.sh <tag> <file under target/src> <tag>.py '<mangled fn>' 0x<retail addr>`.
  It prints ALL and the fn diff line count, and BUILDFAIL on a compile error (22.958% = failed build).
- Land a change only through `sh lab/gate_w18.sh $PWD/w200/<tag>.patch` (ratchet), then commit.

## What paid this session
- Quake 3 / CoD4 (KisakCOD) source shapes: `x >= 0 ? 0 : 1` passed to a bool param, one check per `if` + `return 1`, Kisak loop shapes.
- A value read into its own local before the call (SND_RestoreSoundAlias offset) changes register choice.
- Inline decisions: a call site's count depends on loop exits and enclosing loops; a one-trip local-client loop
  (`for (localClientNum = 0; localClientNum < 1; ...)`) lets retail-size callees inline (CL_MapLoading). See lab/w200/inl_notes.txt.
- Stack slots: Mac 1.3 STABS local lists (claude_client/stabs.py); dead-store vec copies leave retail's unreferenced slots (compass).
- Verified Linux server source (cod2engine/archive/lnxded-1.0, -O0) as shape oracle: lab/w200/srvcmp.py, srvport.py.

## Biggest remaining (lab/w200/slotonly.py <build>/target/build/release)
- CG_DrawTracer 621, ScaleTracer 166: ScaleTracer register params (retail: startWidth in ebx only).
- CG_AddPlayerWeapon 591: RA (retail weaponNum in edi, bViewModel in memory).
- CG_ConfigStringModified 411: one `add` scheduled after `cmp` in retail (lea).
- CG_CalcCrosshairPosition 400, CG_DrawLagometer 393: x87 operand order; lagometer uses screen_placement.inl stand-ins (replace with real inlined calls, see inl_notes).
- Com_ProcessSoundAliasFileLocalization 268, BG_ParseCommands 249: .inl stand-ins + RA.
- Data: two misplaced function statics (cddir in Sys_ScanForCD, rect in Item_CorrectedTextRect) need a vanished /GL callee; do not invent one.
- ~40 `.inl` stand-ins remain (grep '#include ".*\.inl"'); each must become a real call before release.
