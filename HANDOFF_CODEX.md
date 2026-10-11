# Handoff: CoD2MP_s.exe 1.0 client, 2026-10-12

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

Next reviewer: reproduce `../lab/mym.sh`, all five REPORT tests and direct start-ledger ratchet;
inspect data/behavior changes as well as code scores. Full SHA, BSS placement, library provenance,
remaining code gaps and runtime behavior still need work. Do not relabel tie gains as reconstruction.
