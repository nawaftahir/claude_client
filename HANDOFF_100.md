# Handoff: CoD2MP_s.exe 1.0 client, 99.650% -> 100%

Read first: dev/CLAUDE.md (laws), ~/.claude/skills/client-recon/SKILL.md, the last ~80 lines of LESSONS.md.

## State (10-28, commit c3703f6)
- Whole-file same-offset 1693888 / 1699840 = 99.650% from real source. 100% same-offset == SHA-identical file.
- Build: `bash ~/lv.sh TAG [patch...]` (RAM build, result in lab/r2w/TAG/sha.txt). Never symlink into lab/r2w.
- Try a source change: write lab/w200/<tag>.py with `R = [(old, new)]`, then
  `cd lab/w200 && bash try.sh <tag> <file under target/src> <tag>.py '<mangled fn>' 0x<retail addr>`.
  It prints ALL and the fn diff line count, and BUILDFAIL on a compile error (22.958% = failed build).
- Land a change only through `sh lab/gate_w18.sh $PWD/w200/<tag>.patch` (ratchet), then commit.

## Checkpoint: 10-28, Codex session
- Kept, uncommitted: `target/src/qcommon/buildnumber.c` buildInfo[1024] -> [128]. Mac STABS `buf[128]` plus Windows .data end address confirm the size. Gate: RATCHET OK, accepted, protected coverage unchanged at 1,318,789 / 1,408,683 B (93.62%). No new matched code bytes.
- Two private fresh release builds: `lab/r2w/codex100_lay_build128{,b}/target/build/release/`. All section bytes equal main6; .data virtual size now matches retail (19575808), headers gain 2 bytes. Sections stay 1693888 / 1699840 = 99.650%; actual entire-file comparison is 1697852 / 1703936 = 99.642944%, 6084 bytes differ. SHA still differs.
- `shameter.py` ALL excludes headers. 100% ALL alone does not establish whole-file SHA. Normal release also runs `postlink.pl`, the documented 2-byte mixer JE->NOP patch; account for that separately from compiler-source bytes. Broad `postcopy.py` is not used by normal release and cannot count as reconstructed source.
- `lab/mym.sh` NET showed +95575 from a stale mybase.json; this is not this session's gain. good.json before/after is unchanged.
- Root trial `lab/w200/codex100_root/endsession_calls.patch`: 24 stand-ins -> real Com_EndParseSession calls across 12 files, remove noinline and pointer-string stand-in. Builds successfully but .text virtual size grows 96 B and sections fall to 30.697%; draft only, not landed. Next work must reproduce selective inlining, not apply this batch blindly.
- CG_ConfigStringModified: early num==7 return moves the string-offset load below the branch (-10 section bytes); bool condition temp before getter leaves the function unchanged. Both rejected; source remains original. Root attempts under `lab/w200/codex100_root/`.
- Tracer diagnostic: disabling global optimization on CalcTracerFinalScale gives ScaleTracer 0 diff lines / 239 B, but loses the callee match (69 -> 86 B) and shifts layout. Do not land it. Confirms the callee's register-preservation summary affects ScaleTracer's width convention. Evidence/patches under `lab/w200/codex100_tracer/`, builds `lab/r2w/codex100_tracer_*`.
- x87 trials under `lab/w200/codex100_x87/NOTES.md`; no kept function gain. Four real CalcScreenPlacement calls plus removal of its noinline reproduce Lagometer baseline code but disturb other callers/layout. Do not land globally.
- Layout: reversing Miles archive input order does not fix Rich record order. Seven missing C++ TU identities still unproven; do not invent blank TUs. engineState pointer's +8 discrepancy is an open lead, not an accepted fix.
- Cleanup completed: 152 old verified ELF core dumps, 3,110,350,848 allocated bytes (2.90 GiB) freed. Manifest `lab/w200/codex100_root/disk_cleanup.tsv`; source, anchors and proof notes preserved.

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
