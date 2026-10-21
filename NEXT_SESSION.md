# Next session prompt (paste this) - WSL session, state 10-20 evening, see HANDOFF_WINDOWS.md top

Continue the CoD2MP_s.exe 1.0 client recon in ~/cod2/test/dev/claude_client.
State: **92.70% = 1,305,793 B** of 1,408,683 B .text, RATCHET OK, accepted in `target/build/good.json` (10-20).
UNCOMMITTED on top of the 92.00% commit 78852ab: `lab/wip_gap23.patch` == main `git diff -- target/src` (w38-w47 code + layout incl. libcmt retail order, w46/w47 .data/.rdata ownership).
Whole file same-offset **9.629% on the RELEASE exe** (`cd target; make release; python3 ../shameter.py build/release/CoD2MP_s.exe ../../cod2engineold/binaries/windows/1.0/CoD2MP_s.exe`; .data 68.00%; the non-release exe reads ~1-2 points high, do not use it).
Next checkpoint **93% = 1,310,076 B (+4,283)**; commit there (clean-rebuild first). Owner week goal: 100% + SHA (memory project_week_goal_100); full agents allowed.

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
- **sb_encode 0x4de1d0 (3,713 B = the checkpoint alone):** exact Speex 1.1.9 text (lsp `M_PI*((float)(i+1))` + `floor(st->vbr_quality)` float overload) = 6 lines (KEPT in main, upstream text); only residue = w_sig window loop operand role (retail fld st->high[i], fmul st->window[i]). w48: deleting the last stmt `st->first=0;` alone flips it = whole-fn IL decides; next attack = compile order of the inlined speex_encoder_ctl (x7)/speex_encode_native/speex_move (raw vs compiled bodies), real builds only. See LESSONS 10-20 + lab/w4/re_re_w48_s3.md.
- 10-20 cost lesson: w46+w47 ~950k agent tok = +198 B. Hunt a lead yourself first (server-text / upstream-text diffs: /tmp-style srvdiff of near-misses vs lnxded-1.0 src, vendor diffs), then ONE agent on it.
- Scr_GetNextCodepos 0x47d1b0 (real size ~0x31c): retail CALLS AddRefToValue(type,u) 0x476810, ours inlines;
  global noinline = 196->78 but NET -2352 -> make it a call at this site only.
- CG_DrawTracer: why retail treats CalcTracerFinalScale as clobbering eax/ecx/edx (c2 oracle 0x1077067f).

## CI (10-20, working)
- Private image ghcr.io/nawaftahir/cod2-client-ci:1 (compiler + d3d9.lib + dsound.lib + retail anchor; Actions access granted to claude_client). Never put those files in git.
- gate.yml (branch wip-ci): full build + cmp/cov/ratchet vs ci/good.json; matches local exactly (1,307,050). ~3 min clean.
- batch.yml (dispatch on a branch): builds every ci/batch/*.patch in parallel (max 20), per-patch NET + ratchet in the run summary/artifacts. Verified: comment patch NET 0, AddRefToValue noinline -2352 = local.
  Use: commit patches (git diff format, a/target/... paths) to ci/batch/ on a branch based on wip-ci, push, `gh workflow run batch.yml --ref <branch>`, `gh run download <id>`.
- Keep ci/good.json in sync with main's target/build/good.json when the baseline moves.

## State 10-20 night
- Gauge 1,308,199 B = 92.87% (RATCHET OK, uncommitted; 93% = 1,310,076). All LTCG tie fns ON except Scr_ParseGameTypeList.
- Structure landed today: libcmt order, helper dedup (one copy each), 13 early-TU retail empty stubs + cg_newDraw/ui_shared stubs,
  LittleLong __inline. Later-TU stubs (scr_parser, FxCurve, record_dsound, cl_main, win_configure...) flip SV_DirectConnect:
  they need a compensating count fix elsewhere (tie model, lab/w4/re_re_w53_ties.md + re_re_w54_tuset.md).
- Code: 6 Opus agents on CI = 0 B. Remaining residues = per-block register pressure (imul dest: XAnimLoadFile, SND_RawSamples)
  and scheduling ties on identical IR. Next code attack = hook c2's block-local pass (probe lab/w4/pq/pr.sh), not respelling.
- Layout: .data 48 KB band -16 = XCU section merge (see LESSONS w61Y). Whole file 9.59% (release).
- CI: lab/cibatch.sh (status polling), lab/cipatch.sh, lab/sweep_ci.sh; batch artifacts res-* (small) / bin-* (exe+map).

## State 10-21 early morning
- Gauge 1,308,148 (92.86%; tie fns PC_EvaluateTokens/SP_worldspawn off); real excl ties 1,304,546. Commit at 93.08% (7b76f73) stands.
- Release layout: whole 12.64% (was 9.45%), .rdata 45.8% (18.7%), .data 92.1% (69.6%). Behaviour bugs fixed: FPS/stats text colours, voice_current_scaler.
- Overnight CI sweeps (~1,000 mechanical variants: dead inits, dead locals, statement swaps) = 0 code matches.
- Open layout: cl_console .rdata contribution 16-aligned (retail 4) - bisect its constants; small .rdata drift from 0x17380 spread over many TUs; dead scaffolding data at .data tail should be deleted.
- Open code: per-fn compiler mechanisms (see LESSONS 10-20/10-21); only targeted leads, no blind agent waves.

## State 10-21 09:30 (paused by owner)
- Gauge 1,314,017 B = 93.28% (commit after 0e0279f). Today: DObjTraceline, Item_ListBox_HandleKey, CG_DrawInformation,
  SL_GetStringOfLen, CG_StartShakeCamera, mixerGetRecordSource matched; nb_decode 12->8, compass 32->26, CG_AddPlayerWeapon 128->95.
- PAID rules today (skill + deep_brief.txt): named local before a call; goto chains -> for/continue; while(1)-break -> while(cond);
  goto-store tails -> duplicated store+return; extra arm copy adds a const use; err result local + early NULL init (mixer);
  check call targets (toupper/tolower bug).
- Method that paid: ONE deep agent per function, probe/oracle first (lab/w4/deep_brief.txt). Waves/sweeps paid 0.
- Fast relink tools: /home/nawaf/mlw/go.sh (oc/), /home/nawaf/mlw/go_gvo.sh (game/), /home/nawaf/dtl/go.sh, /home/nawaf/gto/go.sh.

## State 10-21 13:50 (uncommitted on top of 59a4f32; full diff in lab/wip_gap23.patch)
- Gauge 1,322,134 B = 93.85%. Whole-file ~12.74%. Plan + log: dev/PLAN_10-21.md.
- Matched today: Record_QueueAudioDataForEncoding (anon struct of int locals = slot order), ScaleTracer (const fn ptr),
  sb_encode +3,713 (ONE no-op cast `((SBEncState*)state)->frame_size` renumbers whole-fn IL -> commutation order).
- Structure kept: qcommon.h Com_EndParseSessionInl non-static (+2,545), lyx_all (dead fns), names_fix rows, CL_DrawText body
  -> cl_console static inline (w87_pr), AngleNormalize360 dup removed.
- PROVEN: 59 of 61 unmatched fns do not move with fn count (lab/w4/cnt_sweep.tsv); residue is per-fn. Cast scan only moved
  commutation (sb_encode); register ties did not move in ~200 valid builds. SV_DirectConnect is a count tie that blocks
  fn removals (keep D in {0,1,3}, lab/w4/sdc_period.tsv).
- Unfinished (WSL overloaded, load 103): ~150 cast variants never built: /home/nawaf/wpc_names.txt, mlw/var/cr*, crr*, k3_*,
  k4*. Re-run at <=4 concurrent builds before calling them dead. BoneBox lines 779-783 untested.
- Parked patches: lab/w87_ac.patch (XAnimCalc 93->6, not server text), lab/w87_tum.patch (compass fn move, NET 0).
- Next real lever: the regalloc oracle (tools_oracle/REGALLOC.md) for the register ties; cast scan only for commutation.

## State 10-21 evening (uncommitted; lab/wip_gap23.patch)
- Gauge 1,322,392 B = 93.87%. Added since 13:50: CG_DrawPlayerCompass +258 (test stored color[3] not local alpha),
  CursorHint register swap fixed via priority formula (100 -> 36 lines, ora_dt3), CL_DrawText body -> cl_console static inline.
- COMPILER RE (tools_oracle/REGALLOC.md rounds 5-12, SCHED.md, C1.md): register pick + priority formula PINNED
  (n3 per region, ties -> larger n17 = IL pos of last ref; lever split f(g(x))); probes output-identical to stock.
  Triage lab/w4/tri.tsv: register ties only 2 fns; scheduling 11 fns 5.4 KB; x87 operand order 4 KB (decided by
  earlier IL in the fn, not expression spelling; PARKED: needs IL-numbering RE); structure 34 fns 18 KB.
- Fast tools: /home/nawaf/mlw/ora/nogl2.sh (no-/GL compile, 20 s/batch), tiny/ltc.sh (small LTCG DLL in seconds),
  bf/ (mechanical variant generator: single edits paid 0).
- Next: structure group (18 KB) with per-fn deep agents + the new tools; scheduler still unfound (not in c2 IL passes).
