# Handoff: CoD2MP_s.exe 1.0 client, 2026-10-12

## Larger-group continuation - actual date 2026-10-11

Latest accepted incremental build: **1,231,862 B / 87.45% matched .text**.
This continuation adds **826 genuine bytes**, XModelPartsLoadFile 0x48dc70.
The function has an exact match and checked direct calls/six string operands;
all sixteen current test_*.py suites pass. The old allocation/accounting
order is rejected by the native allocator-observation fixture.

Clean source is in target/src/xanim: xanim_public.h returns the two-byte
XModelFileShort aggregate from the existing XModelDataReadShort helper;
all eighteen callers in xanim_load.cpp/xmodel_load.cpp use .value.
XModelPartsLoadFile accounts quaternion allocation AFTER its allocator call,
as retail does. No extra helper or public-loader ABI change. This source
form matches; its type/name are not claimed as recovered historical text.

The private-reader prototype gave an intermediate meter of 1,231,246 B
(826 new, known Scr_ParseGameTypeList tie -572, four-hit helper group -44).
Reusing the existing reader removes that fifth copy and restores the group;
neither the 44-B recovery nor the known tie is additional reconstruction.
Current strict accepted ledger is target/build/good.json, SHA-256
f860f1230bcddfc20d2c36edfcacdc1dbcc2b7dd777e42e2a6a69b860836f1be.
Final replay: model_final_gate_1011.log and model_final_tests_1011.log.
It equals the strict replay of the cold model build byte-for-byte.

XAnimLoadFile remains unmatched, but the shared unsigned-reader + existing
final FS_FreeFileInl draft reaches the retail 4053-byte size/frame. Local
temp2/temp in each continuity loop then reduce 14 -> SIX diff lines; only
the delta-quaternion imul/add destination remains different. All drafts are
restored; animation_6diff_1011.patch saves this lead against HEAD (also
contains the accepted signed-reader edits: never blindly apply). Its native
i386 actual-loader/types test passes 20 quaternion/flip fixtures and four
failures, but the function is NOT counted. The apparent protected
8-byte library _Copy EH loss was an out-of-text linker placeholder, not code.
The bounds correction below resolves that false regression. Operand
swapping and statement splitting did not help. Saved animation_14diff_1011.patch
(against HEAD, includes the accepted signed-reader edits) and .txt: inspect
before applying, never apply it blindly over current accepted edits.

Bounded final trials are recorded in deepseek/LESSONS.md, not to be repeated:
casts, const inputs, guards, component caches, index/pointer forms, inline
dot helper and union readers did not finish the delta multiply. The genuine
inline helper also emitted an unwanted 25-byte copy and is removed. A
two-component loop worsens to 212 diffs. Speex open_loop_nbest_pitch pointer
IVs worsen its frame/body and lose two real callers / 5663 B; restored.
None of these trials adds code coverage or changes ties/masks.

A fresh all-253-TU cold verification of the accepted signed-reader source
finished green, model_cold_1011.log. Prior build preserved at
pre_model_cold_1011. No whole-file SHA identity or runtime claim.
Verified source is ready for a local review checkpoint; no push. Renderer remains safely parked.

Comparator repair: cmp.py now rejects map entries outside the actual .text
section. Three linker placeholders previously sliced code using negative
Python indices, one falsely matching eight bytes inside another retail body.
Both saved start and current physical binaries were independently replayed
with the stricter referee. Union coverage remains 1,231,036 -> 1,231,862 B:
those eight bytes were already covered by another genuine match, so removing
the false entry changes neither total. Do not subtract eight from the meter.
The unchanged-ties ratchet reports only the genuine new 826-byte loader.
fdiff.py/alldiff_lib.py apply the same source bounds to avoid selecting the
phantom duplicate for diagnostics. Test test_matcher_bounds.py uses three
actual rows extracted from shared_reader_accepted_1011.json (ledger SHA
71db428942bdae367b3f03bcdf8a1ac272cda997380f7d2801c3e8319da6840f)
as matcher_bounds_legacy.json; no copied machine code or invented golden.
Evidence: model_strict_good,
client_first_strict_start, model_strict_gate and model_strict_tests_1011 logs.
Any pre-fix JSON remains historical evidence, not the new referee baseline.

Comparator speed: disasm_lite avoids per-instruction Python objects in the
same direct-call/jump masking pass. Every instruction/mask stream (7344)
and the entire same-image JSON ledger compare equal with the previous
decoder. Same-image timing 15.32 -> 8.42 seconds; no result cache. Persistent
test_matcher_lite.py checks the current complete image. Benchmark logs are
matcher_lite_1011 / matcher_objects_replay_1011; their image is the rejected
Speex draft, NOT accepted progress. Final accepted source replay above
equals the cold strict ledger; six-line animation source is saved, not live.
Manifest model_final_inputs_1011.sha256 passes. Four changes vs the original
model-cold manifest are explicit: one blank line in xanim_public.h, the
cmp.py/fdiff.py fixes and test_model_loader.py's actual-declaration extraction.
New tests and alldiff_lib.py are also included in the final manifest.

Final /FIXED release link succeeds without unresolved symbols but differs
from retail: 21efabc4b21d176c1c4af3224d06121dbb4e568a8aab8d318f4345424f7e6176
vs 8caf434c9509020b9c94720d0407a31d61411312996dab9964f46865f070900e.
V4/V5 remain unmet; source/metric completion is NOT being claimed.

## Latest continuation - actual date 2026-10-11

Owner cleared the renderer goal and selected client first. Branch remains
codex/1012; do not create it again. Latest existing commit is f5c3adb.
Clean source stays in target/src, research/tests in scratch/codex_1012.

Fresh gated start 1,230,754 B; latest 1,231,036 B (87.39% matched .text),
**282 genuine new bytes**, no real losses. New uncommitted source matches:
CG_LockLightingOrigin 0x4c5cc0/170 and EmitAssignmentStatement 0x46d3f0/112.
Both have actual-body native tests; assignment also validates five call
bindings. See the latest section of scratch/codex_1012/REPORT.md for exact
files, tests and attempts. Do not count the historical 2783-B tie as new work.

Latest numeric-only correction in cg_weapons.cpp restores the retail weapon
movement smoothing minimum: (0.001f * 0.1f), bits 0x38d1b718, and its negative.
The old 0.0001f was one float step too small. Actual-loop and linked-literal
tests pass, the ratchet is green and accepted; no additional code credit.
Removing the redundant outer loop lost two protected functions / 683 B and
was reverted. Two bounded vector-copy batches also kept nothing; all drafts
are restored. Latest total remains 1,231,036 B, not whole-file identity.

Fresh client-first cold checkpoint: all 253 source objects rebuilt from an
empty object tree, with the old build preserved at
scratch/codex_1012/pre_cold_build_client_first_1011. The complete comparison
ledger equals the incremental ledger (SHA 0217a895e6c3f35f228c6b2b28c4cc60db782e6c70597cdb206aa076363f1e5e).
All twelve current test_*.py suites pass; input hashes pass before/after.
The release link has no unresolved externals but is NOT SHA-identical to
retail. Logs: client_first_cold_1011.log, client_first_tests_1011.log,
client_first_release_1011.log; source/tool manifest client_first_inputs_1011.sha256.
This supersedes older cold-build caveats below, not the unresolved V4/V5 gates.

Two bounded batch searches gave no additional accepted changes; all drafts
restored. Earlier source behavior/data fixes are committed through f5c3adb.
Existing vary.py edits remain the owner's unrelated work. No commit or push
this continuation; full SHA and runtime proof are still outstanding.
Renderer recovery is gfx/target/scratch/codex35/proof_checkpoint19, 37.366%
verified game code; its text/source packet is staged for owner review.
The renderer goal is cleared, not active and not completed.

You take over the client reconstruction for today. Claude reviews your branch afterwards; nothing
merges without that review.

## Goal
The end goal is a whole exe that is SHA-256-identical to retail and runs like retail, with every function in real C/C++.
Today's target is the next whole +1%: **1,242,058 B** matched `.text` (88.17%). Start is **1,227,971 B**
(87.17%, commit `e30122f`). The best build seen was 1,230,754 B; two tie functions flip in and out (see Ties).

## Workspace
- Repo `dev/claude_client`, build dir `target/`, source `target/src/`, link order `target/src/tus.txt`.
- First: `git switch -c codex/1012 e30122f`. Commit only on that branch. Base for review = `e30122f`.
- Retail anchor: `dev/cod2engineold/binaries/windows/1.0/CoD2MP_s.exe` (never commit it).
- Toolchain: docker image used by `target/Makefile` (MSVC 7.1 `/O2 /GL`, `link /LTCG`). A build is ~15-60 s.
- Do not touch `gfx/`, `codex/`, `glm/`, `cod2engine/`, `deepseek/` (except appending to LESSONS.md).

## Read first (in this order)
1. `dev/CLAUDE.md`: universal laws (referee, never fabricate, no `_emit`/byte arrays, no defeatism).
2. `dev/deepseek/LESSONS.md`: every tried fix and dead end. Do not retry a logged dead end.
3. `~/.claude/skills/client-recon/SKILL.md`: tools, fix-pattern catalogue, PINNED facts (incl. "Data layout facts").

## Build and measure: the only allowed way
From `target/`:
- `../../lab/mym.sh` (or `../../lab/mym.sh '<mangled@@>' 0xADDR` for one fn's diff-line count).
  Prints `NET`, `exp <bytes>`, then the ratchet. `exp` is the number. Never quote a number you did not just measure.
- `RATCHET REJECT` = a real fn was lost: revert that edit. `RATCHET OK` and kept: `../../lab/mym.sh accept`,
  then `git diff > ../../lab/wip_codex.patch`.
- Never `git checkout` a src file to undo; restore from your patch or undo the edit by hand.
- Never edit src while a build runs. Run builds in the background and prepare the next edit meanwhile.
- One fn diff: `python3 ../fdiff.py build/CoD2MP_s.exe build/CoD2MP_s.map <anchor> '<mangled@@>' 0xADDR 6000`
  (`-` ours, `+` retail). Near-miss list: `python3 ../alldiff.py > scored.txt` (diff-lines size addr name).
- Batch tools: `sweep.py`, `vary.py` (see skill). Data tables: `python3 ../tabcmp.py build/exp.json`,
  `python3 ../tabdump.py OURVA RETAILVA [n]`. Strings/data map: `python3 ../datamap.py build/exp.json out.tsv; python3 ../datadiff.py out.tsv 0`.
- Whole-file image: `make release` (links `/FIXED`, no .reloc, prints both sha256). Last run 10-06: 1,699,840 B vs retail 1,703,936 B.

## Ties
LTCG re-rolls a few functions on any symbol-count change, including data-only edits. They are listed in
`dev/lab/ties.txt`. Losing one is not a regression and gaining one is not progress. Judge an edit by its
own function's diff lines. Currently lost: SV_ArchiveSnapshot (2,783 B) and Pickup_Weapon (1,631 B).

## Queue (concrete, retail-pinned; do in order)
1. **Scoreboard lead team**: retail keeps it as a function static initialised to 2 (`.data` tail 0x59b110,
   used by the fn at 0x4d4490 = our `CG_DrawScoreboard_ScoresList`, cg_scoreboard_mp.cpp:692). Ours is a
   zero bss global (`globals.cpp:144 int lastLeadTeam;` + extern at cg_scoreboard_mp.cpp:39). Make it
   `static int lastLeadTeam = 2;` inside that fn. This is also a behaviour bug.
2. **Third-person camera bounds**: cg_view_mp.cpp:11-12 globals `maxs`/`mins` are function statics in retail
   (0x59b0f8 maxs {4,4,4}, 0x59b104 mins {-4,-4,-4}) inside `CG_OffsetThirdPersonView`. Declare inside it
   in Q3 order `mins` then `maxs` (statics land in reverse declaration order); remove the globals and externs.
3. **tabcmp leftovers** (each: verify with tabdump first, then fix the source order/content):
   retail has an extra 0 dword after `g_sound_channels` before the room table; the shellshock dvar-name
   table (ours 0x598b10 vs retail 0x596310) is shifted one entry; the l_precomp "on"/"off"/"if" table
   (ours 0x58d424 vs retail 0x58c37c) is shifted; FX vtable order drifts. Skip the CRT rterr table (libcmt version).
4. **Code % (the gate)**: 300 unmatched fns / 126,917 B. Run alldiff, then sweep on <=24-line near-misses,
   then hand work on structural diffs only (missing/extra calls, split or merged helpers, wrong statics).
   10 minutes or 2 builds per fn, then log one LESSONS line and move on.

## Hard rules
- Never fabricate a name, constant, offset or type. Unknown fn = `sub_ADDR`-style name with the address.
  Tag non-obvious facts PINNED (seen in retail) / SUPPORTED / HYPOTHESIS (never ship).
- Real C/C++ only: no `_emit`, inline asm, byte arrays, retail-byte copies, post-link patching.
- Skip 0x4c6e30 and 0x4c6f30 (open owner question).
- Don't retry the parked ones: CL_MapLoading (shrink-wrap), CG_OffsetThirdPersonView fmul order,
  UI_DisplayDownloadInfo (tail merge), nb_decode, the scr Scr_ResetTimeout family.
- Comments: one short `//` line, ASCII, only where the code can't say it. No stacked `//`.
- Git: identity `Nawaf` as configured, short messages, **no Co-Authored-By line**, never push.
  Commit a working step when RATCHET OK; never commit `.obj`, exe, DLL, Ghidra, assets, `src/core.*`.
- Edits via exact-match replacement (assert one occurrence). Don't reformat files you didn't change.
- Append one line per win or dead end to `dev/deepseek/LESSONS.md` (`- 10-12 <fn>: what, result`).

## Report back (append to the bottom of this file)
Start and end `exp` bytes (from mym.sh, same session), commit list, what paid, what was parked and why,
and any new PINNED fact. Claude will re-measure the branch from a clean build and diff it against `e30122f`.

## Report
Codex checkpoint, actual environment date 2026-10-11 (the original handoff label says 10-12).

- Branch `codex/1012`, base `e30122f`; Claude remains the independent reviewer, sequential ownership.
- Source/test checkpoint commit: `d191273` (`client: restore retail state and data behavior`).
  The following evidence-only commit records this ID and removes blank diff-context whitespace.
- Fresh start: **1,227,971 B**. End incremental AND cold rebuild: **1,230,754 B**.
  +2,783 B is only the known SV_ArchiveSnapshot tie. **Zero new real code credit**;
  requested 1,242,058 B has NOT been reached. No real matches lost and no metric/gate changes.
- Kept source fixes: scoreboard initial lead=2 and camera function statics; correct capture ring wrap;
  compass fade else scope; exactly 29 shellshock save/load names; typed 72-byte screen storage.
  Source is in `target/src/`; detailed evidence/tests are in `scratch/codex_1012/REPORT.md`.
- Fixed the existing untracked `splitsw.py` generator's branch scope. Isolated patch retained as
  `scratch/codex_1012/splitsw.patch`; no bulk import of previously untracked tooling.
  It is a zero-context diff: use `git apply --unidiff-zero` if applying to the original tool;
  reverse check with that flag passes against the current fixed tool. Do not apply it twice.
- All 253 TUs rebuilt into an empty build tree, cold ledger exactly identical to incremental.
  Five targeted native/source/linked-data tests pass; direct ratchet against starting ledger passes.
  Old build preserved in `scratch/codex_1012/pre_cold_build/`; no artifacts deleted.
- Release link succeeds without unresolved symbols but SHA still differs; Windows smoke not run.
  See REPORT for both release hashes. This is a review checkpoint, NOT a release claim.
- Parked: MantleHint x-parameter reuse (4->6 diff, reverted); capture scheduling after two correct
  source forms; table scanner overruns. Room/month/script table shifts can be neighboring-object
  layout or pairs starting inside a record, not wrong entries. Pin actual boundaries before editing.
- Existing unrelated `vary.py` change preserved. No gfx/codex/glm/release monorepo edits; no push.
- Confirmed facts/dead ends appended to `deepseek/LESSONS.md` and the client-recon skill.

Next reviewer: reproduce `../lab/mym.sh`, all twelve current REPORT tests and direct start-ledger ratchet;
inspect data/behavior changes as well as code scores. Full SHA, BSS placement, library provenance,
remaining code gaps and runtime behavior still need work. Do not relabel tie gains as reconstruction.
