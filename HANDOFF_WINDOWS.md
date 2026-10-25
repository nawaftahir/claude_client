# Handoff: client recon (state 10-20 evening; Windows session picks up here, then NEXT_SESSION.md)

## Pick up here (10-20 evening)
- Main `claude_client/` = **1,305,793 B = 92.70%**, RATCHET OK (`target/build/good.json`). Release same-offset 9.629%.
  UNCOMMITTED: `lab/wip_gap23.patch` == `git diff -- target/src`, on top of commit 78852ab. 93% = 1,310,076 (+4,283).
  Never `git checkout` src; restore from the patch. Commit only at 93% after a clean rebuild (Nawaf, no Co-Authored-By, no push).
- Today: libcmt `__CI*` order = retail (+915 real; `lab/ci_t2_cfg.patch`: com_math `tan(float)`, SinSquaredDeg
  `sin(angle * k)`, Com_EndParseSessionInl as `static __forceinline` in qcommon.h + 2 TU copies). Ties -602 net.
  CG_DrawLagometer now calls real CL_DrawStretchPic (fake Inl deleted, NET 0). No other bytes today.
- **Tie pool is the biggest open number.** Today's state: PC_EvaluateTokens 2587, SV_DirectConnect 2322,
  SV_Netchan_PrintProfileStats 2061, Scr_ParseGameTypeList 572 ON; Pickup_Weapon 1631, SP_worldspawn 458,
  CG_FireWeapon 550 OFF. Reverting the Com_EndParseSessionInl move swaps the two sets (NET -5,453, tested).
  Ties follow the NUMBER of emitted fns in early TUs (LESSONS 10-19 w42). Getting all ON = +2,089 to +2,639.
  Attack: remove our non-retail emitted fns (list: `python3 ../lab/extras.py` in claude_client/target (needs ~/sp/scored46.txt or edit its path); logic =
  unpaired fn of ours whose matched neighbours sit closer in retail). Known extras: FS_UseSearchPath,
  FS_ReadFileInl, FS_FreeFileInl (com_files, TU 30), Com_EndParseSessionInl copy (ui_gameinfo_mp),
  Field_CompleteCommand + 3 more (common), LittleLong copies (net_chan), XAnim_* template copies,
  FxMemMgr Free/MoveToFront x10, AngleVectorsInl (g_items_mp; real AngleVectors = -1132, keep).
  Each removal must keep its host matched (LTCG has to inline the real fn like retail). 1 build each, 3 slots.
- Remaining real bytes: 65 paired unmatched fns (44 KB), all 5+ attempts, mostly sched/regalloc residue.
  Least-tried: SND_RawSamples 248 (imul dest), speex_bits_pack 176 (block layout), mixerGetRecordLevel 365,
  CG_DrawGrenadePointer 367 (8 lines), CG_AddMovingTracer 184 (2 lines). Biggest: XAnimLoadFile 4053 (4),
  sb_encode 3713 (6, speex 1.1.9 PINNED by retail string), nb_decode 3239 (10).
- Dead today (LESSONS 10-20): CG_DrawPlayerCompass 9 forms (incl. CG_CompassCalcDimensions call, commuted
  operands, rect->h read first); FX_CalcOriginAndAxis yaw reuse / angle local (norm 12-26 vs 0; retail has one
  never-referenced slot at esp+0x30 above the FastSinCos pSin home, cause unknown); SL_GetStringOfLen regalloc.
- Slots s3/s4/s6 = main src minus today's lag patch (s3 also has the Com_EndParseSession revert): rsync before use
  `rsync -rc --delete --exclude='core.*' claude_client/target/src/ lab/sN/target/src/`, make, cmp.py -> base.json.
- Tools: gate `sh lab/gate_w18.sh <ABS patch>`; slot measure `sh lab/smeasure.sh sN ['mangled@@' 0xADDR]`;
  lost/gained `python3 ../../lab/lg.py` (run in a slot target; build/exp.json vs base.json); stack-normalised diff `python3 lab/s6/norm.py diff.txt`.
- Run git/builds through `wsl.exe -d Ubuntu -- bash <script>`; builds never from the Windows side.
- Budget: owner is short on usage. No agent wave without a concrete lead found cheaply first; a wave under
  2 B/ktok gets no follow-up. Report .text %, release same-offset %, tokens vs bytes.

Target: `CoD2MP_s.exe` 1.0. End goal: every function real C/C++, whole exe
SHA-256 identical to retail, runs like retail. % of `.text` is only the meter.

## State

- Main = this repo, `target/`. Measured **1,306,197 B = 92.73%** of 1,408,683 B (RATCHET OK,
  `target/build/good.json` 10-19 22:19), UNCOMMITTED (39 src files; backup `lab/wip_gap23.patch`).
  Last commit 92.00%. Next checkpoint 93% = 1,310,076 B (+3,879). Release whole-file same-offset 9.50%
  (`make release` + shameter.py), libcmt member order = retail since w42.
- Start with NEXT_SESSION.md (state, effort split, do-not-repeat list, next leads).
- ~95 unmatched fns left (~55 KB); every one has 5+ logged attempts. Whole-file
  SHA distance (last `shameter.py`): 7.8% same-offset bytes, 574 layout drift
  points (`drift.py`); 5 of 6,588 matched fns sit at their retail address.
- 10-19 PAID CLASSES (skill catalogue top): x87 operand-role rule (dead `float x = 0.0f;` init, `(double)` product cast under /Op); field pointer `int *f = &s->field[0];` keeps store/load source order; whole-fn server text for shared bg code; float overload `atan2((float)a,(float)b)`. Older 10-19 levers: separate early returns, element pointer
  read at each use, biased IV = `p = &base[i]`, struct copy through a pointer +
  field re-set, `x += w; x -= c;` split, explicit `return v;` closing an arm.
- Toolchain fixed today: image `cod2-msvc71-vs2003`, genuine VS2003 compiler +
  its own Platform SDK under Wine 5. A clean build reproduces 1,273,258 exactly,
  and all 286 import hints equal retail. Read `toolchain/README.md`.
- 51,066 B of `.text` is int3 padding: matched-function ceiling is ~96.4%.
  Counting padding is an open owner decision; don't count it.

## Files you need that are not in git

The workspace lives in WSL: `\\wsl.localhost\Ubuntu\home\nawaf\cod2\test\dev\`.

| What | Path under dev/ |
|---|---|
| Retail anchor exe | `cod2engineold/binaries/windows/1.0/CoD2MP_s.exe` |
| Compiler payload (29 MB) | `sdk/vs2003/msvc710_vs2003.tar.gz` (sha256 in `SHA256SUMS`) |
| DX SDK libs | `sdk/sum2004/` (Makefile `DXSDK`) |
| Original VS2003 disc 1 | `sdk/vs2003/VSPROD1.7z` |
| Lab tools (gate, ratchet, slots) | `lab/` (`mym.sh`, `ratchet.py`, `ties.txt`, `wip_gap23.patch`, `gapr.py`, `ra_try.sh`) |
| Lessons log | `deepseek/LESSONS.md` (read the triage list + every 10-18 line first) |
| Skill | `~/.claude/skills/client-recon/SKILL.md` |

Build needs Docker with the image (`toolchain/README.md`, quick path). Never use
the old `cod2-msvc71-build` image (Wine 8 hangs, wrong SDK).

## Loop

1. Edit source in a slot (`lab/sN/target`, rsync'd from main), 2-3 builds per
   function, fdiff: `python3 ../fdiff.py build/CoD2MP_s.exe build/CoD2MP_s.map <anchor> '<mangled@@>' 0xADDR 6000`.
2. Apply to main, gate: `cd claude_client; ../lab/mym.sh ['<mangled@@>' 0xADDR]`.
   Keep only on RATCHET OK, then `../lab/mym.sh accept` and
   `git diff -- target/src > ../lab/wip_gap23.patch`.
3. Never `git checkout` main src (restore from the patch). Ties are never
   progress. Log every dead end in LESSONS.
4. Commit only on a whole +1%, identity Nawaf, short message, no Co-Authored-By.
   Never push without the owner saying "push it".

## Open leads

- `CG_DrawCompassFriendlies` 0x4d4af0 (2,976 B, ~40 lines with stack offsets
  normalised by `lab/s6/norm.py`): retail shares one &s/&c FastSinCos slot pair
  across both loops (ours one pair per loop) = the 8-byte frame gap 0xa4/0xac.
- Layout for SHA: `lab/layout_helpers_inline.patch` (one copy per header helper)
  is ratchet OK but flips ties Pickup_Weapon/SP_worldspawn (-1,517 on the meter).
- CRT `__CIsin/__CIcos/__CIacos` are code-identical; they jump into shared CRT
  code placed differently = link-order work, not source.
- `Com_ProcessSoundAliasFileLocalization` 0x42f8e0: pure regalloc. f's first
  live piece has gain -2 in c2's colour pass (rejected at 0x10782a5e), so esi
  goes to the CSE'd `"soundaliases/temp.csv"` literal. Untested: `if (f == 0)` /
  `if (f <= 0)`, an un-poolable literal. Oracle: `lab/ra_try.sh` +
  `tools_oracle/REGALLOC.md`.
- `VM_Notify`: do/while dup-arm form = retail order; residue is a size/type
  eax<->ecx swap (LESSONS 10-18).
- Fresh targets: `python3 alldiff.py > scored.txt`, then `frame.py` / `andesp.py`;
  `lab/gapr.py build/good.json` for the gap inventory.
- Not done yet for whole-exe SHA: data layout, bss gap, the mixerGetRecordLevel
  post-link NOP patch (owner decision).

## Coming back

Commit to a branch here (e.g. `win/1018`) and push it; the Linux session
reviews against `good.json` with `lab/mym.sh` before merging.
