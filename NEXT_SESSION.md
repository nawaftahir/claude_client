# Next session prompt (paste this) - WSL session, state 10-20 06:30

Continue the CoD2MP_s.exe 1.0 client recon in ~/cod2/test/dev/claude_client.
State: **92.73% = 1,306,197 B** of 1,408,683 B .text, RATCHET OK, accepted in `target/build/good.json` (10-19 22:19).
UNCOMMITTED on top of the 92.00% commit 78852ab: `lab/wip_gap23.patch` == main `git diff -- target/src` (39 files: w38-w43 code + w38/w42 layout incl. libcmt retail order).
Whole file same-offset **9.50% on the RELEASE exe** (`cd target; make release; python3 ../shameter.py build/release/CoD2MP_s.exe ../../cod2engineold/binaries/windows/1.0/CoD2MP_s.exe`; .rdata 29.01%, .data 65.18%; the non-release exe reads ~1-2 points high, do not use it).
Next checkpoint **93% = 1,310,076 B (+3,879)**; commit there (clean-rebuild first). Owner week goal: 100% + SHA (memory project_week_goal_100); full agents allowed.

Read first: `dev/CLAUDE.md` "Client track rules", HANDOFF_WINDOWS.md (state + open leads),
the client-recon skill fix-pattern catalogue (top ~12 entries = 10-19 PAID classes), LESSONS.md tail.

## Rules (owner)
- Make technical calls yourself (real C/C++, identical behaviour, identical SHA); do not ask the owner
  to choose. Ask only before push (needs "push it"), servers, destructive actions.
- Every wave = code agents + ONE Sonnet layout agent in its own slot. Every update reports:
  .text %, whole-file same-offset %, tokens spent vs bytes gained.
- Commit at whole +1% (`git -c user.name=Nawaf`, no Co-Authored-By), then
  `cp target/build/cmp.json target/build/cmp_base.json`.

## Fastest loop so far (w38, 10-19 night: +7,254 B real in one 30-min wave, ~22 B/ktok)
0. `cd target && python3 ../alldiff.py > ~/sp/scored.txt` (~5 min, background), then `sh ../lab/classscan.sh ~/sp/scored.txt`:
   groups near-misses by diff kind (x87-only lines vs integer/register). One Opus agent per group,
   biggest bytes-per-diff-line first, 3-6 fns each, 45-min box. Plus one Sonnet layout agent (s6).
1. Levers that paid in that wave (skill top): copy the reference text EXACTLY incl. redundant parens
   (server 1.0 SHA src for bg/game/xanim, Q3, Kisak, Speex); open-coded vector math -> the reference
   helper calls (TU-local static inline where the header is missing).
2. Gate both patches, then if main and a fresh slot disagree, `mv target/build/obj` aside and rerun
   `../lab/mym.sh`: incremental main was STALE once (1,297,173 vs clean 1,302,739, tie fns).
3. Check waiting with `until grep -q ... log` only; never `pgrep -f <pattern>` inside the same command (it matches itself and never ends).

## Loop that paid earlier 10-19 (~5 B/ktok vs ~1 for blind waves)
1. Find a SHARED CAUSE first (scan diffs for one shape across fns), then send one Opus agent per cause
   with 4-5 fns that show it. Brief: `lab/w4/prompt_w18_common.txt` (tools, rules, deliverables).
2. Slots: `nohup sh lab/rebase.sh sN > ~/sp/rbXX_sN.log 2>&1 &` (detached, ~3 min; a plain `&`
   dies with the shell). Agents wait for "unique bytes" in the log, then do one unedited build first.
3. Gate each patch into main: `sh lab/gate_w18.sh <ABSOLUTE patch path>` (applies, builds, ratchet,
   keeps or reverses). Log every result in deepseek/LESSONS.md.
4. Agents edit through Windows tools can turn files CRLF; slot diffs must stay LF.

## Open leads (best first)
- Window_Paint 0x533bd0 (26): only real hunk = fillRect reload order after inlined UI_DrawRect (retail w,h,y; ours y,h,w); untried: reverse fieldwise copy.
- Item_ListBox_HandleKey 0x536760 (42 lines): w35 field-pointer class partly applied; key 156
  re-reads listPtr->cursorPos (retail reuses ecx), key 166 tail placement.
- CG_DrawCompassFriendlies 0x4d4af0 (334 raw, 32 normalised): frame 0xa4 vs 0xac, store/fild sched.
- CG_PredictPlayerState_Internal 0x4d85b0 (4): retail makes GLOBAL pe[2] the x87 memory operand vs
  local delta[2]; 29 variants dead, /GL probe agrees with ours. Needs a new rule.
- G_RegisterWeapon 0x5197a0 (30): difference is inside LTCG-inlined G_XModelBad/G_OverrideModel.
- Layout: boxVerts - retail has a writable .data copy + const cg_ents_mp .rdata copy; find both retail
  xrefs and the .data slot BEFORE splitting (blind split: file 7.86 -> 7.56%, rejected).
- Big near-misses already heavily tried (check LESSONS before touching): XAnimLoadFile, sb_encode,
  nb_decode, BG_CheckProneValid, PM_Footsteps, XAnimCalc (server-text conflict, rejected), VM_Notify,
  DObjTraceline, FX_CalcOriginAndAxis.

## Effort split (owner OK 10-19)
1. Main effort: .text matching (Opus agents, aim at shared causes).
2. Alongside: ONE Sonnet layout agent on STRUCTURE only (link/lib order, data ownership, string order);
   a layout fix that re-rolls matched fns goes in together with their re-match (e.g. w40 libcmt order +
   SV_DirectConnect, lab/w40_s6_layout.patch). Measure layout on the RELEASE exe only.
3. Exact offsets/padding/headers + SHA check only after .text is 100%.
- Hidden near-misses: names_fix.tsv 'drop' rows hide fns from alldiff; check lab/gapr.py 'other' bucket.

## Do not repeat (10-19 night)
- Before targeting a fn, check build/good.json: an entry with non-empty "hits" is ALREADY matched; names_*.tsv
  addresses can be mis-paired (CG_ResetEntity/UpdateShellShock/Sys_Error/Item_YesNo_Paint were matched).
- Run slot rebases in the FOREGROUND (`sh rebase.sh s3 & sh rebase.sh s4 & wait`); nohup ones got SIGINT-killed.
- Judge a partial on MAIN numbers, not a stale slot (w41 RunXModelAnims).
- Layout numbers only from the RELEASE exe (`make release` + shameter.py): release 9.50% now.
- Rule studies on 1-insn ties paid 0 B (w40 x2, w44); go for concrete fixes. Brief = 25-min box, 3 builds/fn.
## Next concrete leads
- Scr_GetNextCodepos 0x47d1b0 (real size ~0x31c): retail CALLS AddRefToValue(type,u) 0x476810, ours inlines;
  global noinline = 196->78 but NET -2352 -> make it a call at this site only.
- CG_DrawTracer: why retail treats CalcTracerFinalScale as clobbering eax/ecx/edx (c2 oracle 0x1077067f).
