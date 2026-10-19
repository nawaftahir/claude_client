# Next session prompt (paste this) - WSL agent takeover, 10-19

Continue the CoD2MP_s.exe 1.0 client recon in ~/cod2/test/dev/claude_client.
State: **92.00% = 1,296,031 B** of 1,408,683 B .text (RATCHET OK, `target/build/good.json`, pushed).
Whole file same-offset ~7.86% (`shameter.py`), drift 578 points (`drift.py`).
Next checkpoint **93% = 1,310,076 B (+14,045)**.

Read first: `dev/CLAUDE.md` "Client track rules", HANDOFF_WINDOWS.md (state + open leads),
the client-recon skill fix-pattern catalogue (top ~12 entries = 10-19 PAID classes), LESSONS.md tail.

## Rules (owner)
- Make technical calls yourself (real C/C++, identical behaviour, identical SHA); do not ask the owner
  to choose. Ask only before push (needs "push it"), servers, destructive actions.
- Every wave = code agents + ONE Sonnet layout agent in its own slot. Every update reports:
  .text %, whole-file same-offset %, tokens spent vs bytes gained.
- Commit at whole +1% (`git -c user.name=Nawaf`, no Co-Authored-By), then
  `cp target/build/cmp.json target/build/cmp_base.json`.

## Loop that paid 10-19 (~5 B/ktok vs ~1 for blind waves)
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
