# Handoff: client recon (state 10-19 night; Windows session picks up here, then NEXT_SESSION.md)

## Pick up here (Windows, 10-19 night)
- Main `claude_client/` = **1,303,591 B = 92.54%**, clean-rebuild verified, RATCHET OK. Same-offset 10.606%.
  UNCOMMITTED: `lab/wip_gap23.patch` == `git diff -- target/src`. Do not commit below 93% (1,310,076, +6,485); never `git checkout` src.
- Run git/builds through `wsl.exe -d Ubuntu -- bash <script>`; agent brief template for Windows tools: `lab/w4/prompt_w18_common.txt`.
  Linux-side briefs from tonight (copy + edit wave/slot/targets): `~/sp/p39_s3.txt`, `p39_s4.txt` (code), `p39_s6.txt` (layout).
- Slots s3/s4/s6 are synced to 1,302,739-era main; resync before use: `nohup sh lab/rebase.sh sN > ~/sp/rbXX_sN.log 2>&1 &`.
- Fastest loop (w38, ~22 B/ktok): rescore `python3 ../alldiff.py > ~/sp/scored.txt`, group with `sh lab/classscan.sh ~/sp/scored.txt`, one Opus agent per shared cause, one Sonnet layout agent; gate with `sh lab/gate_w18.sh <abs patch>`; if main and a fresh slot disagree, move `target/build/obj` aside and rebuild (incremental main went stale once).
- Agents: check `grep -c "unresolved external" build/oclink.log` == 0 before trusting smeasure (a /FORCE link fooled the w39 layout agent).
- Tonight's wins (10-19 night, +7,560 B from 92.00%): BG_CheckProneValid, PM_Footsteps, CG_PredictPlayerState_Internal, BG_CalculateWeaponPosition_Sway, SND_SetChannelInfo, PM_FootstepEvent, CL_AddDebugLine, CL_AddDebugString, FX_AddCameraShake; layout 8.24 -> 10.61%.
- Levers (skill top): exact reference text incl. redundant parens; reference helper-call form for vector math; block-local vec3_t; exact header `inline` names (no new symbols); Mac `_this` = C++ method (FxRange::GetVal/SetRange).
- Best next leads:
  - nb_decode 0x4e2b50 (3,239 B, 10 lines): vocoder loop 0x4e34c0, `innov` base/index order [eax+ecx-4] never moved (lab/w4/re_re_w38_s4.md).
  - XAnimLoadFile 0x48bec0 (4,053 B, 4 lines): `imul edx,edi` operand order; decl order dead.
  - CG_CalcCrosshairPosition (gunYaw stack copy), CG_CheckHudStanceDisplay (one more stanceFadeTime read), FX_AddFxToScene, CG_DrawAdsOverlay, FX_RandomizeRoll, CG_DrawLagometer/CG_DrawDevString (shared CalcScreenPlacementInl).
  - Layout: libcmt member order (retail pulls stricmp/strnicmp/strlwr/strupr late) unblocks retail fn 0x401ec0, the 0x401f20 copy of Com_SurfaceTypeToName (needs its real source with the real infoParm_t type) and the <RESET>/<FAST> + default.menu strings.

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
