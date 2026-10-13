# Client correctness checkpoint - 2026-10-11

Review branch: `codex/1012`, source base `e30122f`. Renderer work stays paused.
This is reconstructed source repair, not game bug patching or a finished release.

Latest accepted continuation: **1,231,862 B / 87.45%**, including a new full
826-byte XModelPartsLoadFile match. Earlier totals below are historical.
The accepted source has sixteen passing suites and a fresh cold rebuild.
The saved start/current binaries both passed a stricter referee replay after
excluding three non-code linker placeholders; union totals are unchanged.
See HANDOFF_CODEX.md for evidence and the large animation-loader lead.

Final source replay with the stricter, faster referee equals the cold strict
ledger exactly (f860f1230bcddfc20d2c36edfcacdc1dbcc2b7dd777e42e2a6a69b860836f1be).
Only +826 genuine new bytes vs this continuation's saved start; the older
lighting/assignment changes add 282 in the preceding checkpoint, not again.
Build/test/hash logs: model_final_gate_1011, model_final_tests_1011 and
model_final_hashcheck_1011. Canonical source contains no unsigned-reader,
continuity-loop, union-reader, inline-dot-helper or Speex pointer-loop draft.
Animation's best saved draft is animation_6diff_1011.patch: only one delta
multiply/add destination remains, NOT a match. Native i386 coverage adds
20 quaternion/flip fixtures and four failures against actual source/types.

Mechanical checker speedup preserves all 7344 instruction streams/masks
and byte-identical full JSON on the same pinned image: 15.32 -> 8.42 seconds.
This is time saved, not code coverage. All masks/search/import rules are
unchanged; the bounds correction strengthens validation. The benchmark
image was the rejected Speex pointer-loop draft, restored afterwards.
Source/tool manifest is model_final_inputs_1011.sha256; original cold source
differs only by a blank line in the header, explicitly changed checker/tools
and test reader extraction. No whole-file identity or Windows smoke claim.

## Changes

- Scoreboard lead-team state starts at 2, inside its owning function (retail 0x59b110).
- Camera mins/maxs are function statics in the retail relative order (0x59b0f8/0x59b104).
- Capture-ring wrap no longer subtracts the read offset twice (0x4b7420).
- Enemy compass fade keeps both assignments inside the retail else branch (0x4d4af0).
  The final alpha cap hides the old error for usual caps <=1; do not oversell its runtime impact.
- Shellshock save/load table contains exactly the 29 retail names (0x596310).
  `cg_shock_viewKickFadeTime` still exists as a dvar, but is absent from this table.
- Screen-placement storage is the existing 72-byte structure, not a one-byte array.
  Retail 0x449bd0 confirms every accessed field offset; surrounding retail BSS layout is unresolved.
- `splitsw.py` emits one braced statement, preventing split assignments escaping branches/loops.
  That pre-existing tool is untracked; its isolated change is saved in `splitsw.patch`.

Source changes are in `target/src/`; experiments, evidence and tests stay here in scratch.
Unrelated existing `vary.py` modifications are preserved and excluded from the checkpoint.

## Evidence and accounting

Initial fresh link/measure: **1,227,971 bytes (87.17%)**.
Incremental and clean rebuild result: **1,230,754 bytes (87.37%)** with `RATCHET OK`.
All 253 listed source objects were rebuilt from an empty build tree; comparison ledgers are identical.
Previous build artifacts remain recoverably stored in `pre_cold_build/`, not deleted.
The +2,783 bytes are solely the known SV_ArchiveSnapshot compiler tie: **zero new code credit**.
Direct ratchet against the start also passes; none of the earlier real matches were lost.
No matching rule, tie list, relocation mask or goal was weakened.

Five native/source/linked-data tests pass:

```
python3 scratch/codex_1012/test_capture.py
python3 scratch/codex_1012/test_compass.py
python3 scratch/codex_1012/test_statics.py
python3 scratch/codex_1012/test_shock_table.py
python3 scratch/codex_1012/test_screen_storage.py
```

Capture fixtures also reproduce the original double-subtraction bug from the base git blob.
These are targeted tests, not a Windows game smoke test or proof of every caller/data binding.
Release-mode link succeeds without LNK2001/LNK2019, but its SHA does NOT equal retail:
`ca4545f3c7d232156cf5510046094fe07841b16aefb7a53f3f6ac9dda7b48df7`
versus anchor `8caf434c9509020b9c94720d0407a31d61411312996dab9964f46865f070900e`.

## Parked / next

MantleHint x-parameter reuse worsened 4 to 6 diff lines; reverted. Capture codegen remains
unmatched after two correct source forms; retain the behavior fix, park scheduling experiments.
Do not reorder the l_precomp directive table from tabcmp overruns: retail 0x58c37c begins
with literal space bytes. Bound FX vtable slots before interpreting those diagnostics.
Remaining work includes real code gaps, data/TU placement, library/toolchain provenance,
full-file SHA identity, and a Windows boot/smoke run. The requested 88.17% target is not reached.

Cold ledger SHA-256: `26318e35f312fdd2a1dc27a91be1ca0c73e356d9031e89d7d476ba0dccf9acd6`.
Cold dev EXE SHA-256: `9ef03b3f99634c0cc2ff86d0c026b4deb8f5909162bad7e30bbde181802be41f`.
Toolchain image: `sha256:17ddd3cc30517bc070c4e1a1d8bec9c4abd0d8b453115dfa0324e7e20dc94131`.
No retail bytes were copied into code; no canonical release tree changes and no push.

## Continuing source/data work, 2026-10-11

This section follows the earlier cold checkpoint; it is not another cold-build claim.
Two batched parameter-reuse probes, two UI loop forms and one expanded VectorMA form
gave no real code matches and were reverted. The existing vary.py change is untouched.

The next literal audit checked 4,337 memory operands in uniquely matched functions.
It found real data errors hidden by relocation masking. Kept corrections in target/src:

- Sound selection: both probability multipliers are +32768.0f, not -2147483648.0f.
- Degrees-per-radian: the retail float is 57.2957763671875.
- Scoreboard header baseline: the retail float is 11.75999927520752.
- Camera zoom numerator: retail lifts float 0.69813174f to double for tan.

The final fresh build still measures **1,230,754 B (87.37%)**, with direct start-ledger
ratchet green. These changes fix numeric behavior, not new .text coverage. The earlier
cold-build hashes describe that earlier checkpoint, not these modified sources.

Additional tests:

```
python3 scratch/codex_1012/test_sound_selection.py
python3 scratch/codex_1012/test_retail_constants.py
python3 scratch/codex_1012/audit_real_constants.py
```

The actual selector passes null/single, both two-alias choices and three-alias sequence
fixtures; its old negative multiplier fails the first selection fixture. Eight linked
functions use the pinned numeric operands, including both ambiguous asin/acos hits.
All five previous tests also pass. Another 49 literal sites in multi-hit functions
have no unanimous numeric contradiction.

The literal audit leaves one diagnostic in CL_RunOncePerClientFrame: ours commutes
timescale/1000 between fdivr and fmul; both values exist, so it is not a missing constant.
A direct-global source probe did not change it and was reverted. This operand ordering
still needs exact reconstruction; zero-filled initial timescale storage is not its value
after initialization. No runtime smoke or new whole-file identity is claimed.

Nine bounded effects vtables show no contradictions in known matched slots. This does
not prove unmatched slots. SCR/ROQ_RunCinematic candidates are 0x4034d0/0x4aff30, not
the misleading DB address 0x4143c0; current wrappers still tail-jump instead of call/pop.

The subsequent call audit confirmed three more source corrections, still at 1,230,754 B:
CL_Init calls the existing CL_ClearState, rename notifications use plain CG_GameMessage,
and HUD clock case blocks precede material. Targeted linked checks validate four call PCs
and all twelve HUD type dispatches (byte remap plus relative jump-table destinations).
Native init fixtures reject the old skeleton-only call and retain the five required flags.
The retail state-clear export from Ghidra omits those flag saves; actual machine code proves
the existing source's saves/restores are correct, so the state-clear body was not changed.

```
python3 scratch/codex_1012/test_client_init.py
python3 scratch/codex_1012/test_call_bindings.py
```

Broad bindcheck candidate counts: 5,109 ok / 2,959 unknown / 119 bad. These are candidate
counts, not source coverage: most bad candidates have another valid hit. The only remaining
all-bad names are two short decrement-iterator pairings incorrectly assigned to 0x4abb40
(_Inc); actual _Dec wrappers are 0x4abb50 and 0x4ac590. Those DB corrections remain research,
not accepted source gains. The call checker does not validate every data operand or unknown
callee. No claim of complete binding verification is made.

## Client-first resumption - 2026-10-11

Continuation: the unmatched-literal diagnostic exposed two genuine coefficient
errors in CalculateWeaponPosition_BasePosition_movement (retail 0x4cf1d0).
Retail 0x583350/0x58334c contain float bits 0x38d1b718/0xb8d1b718, produced
by 0.001f * +/-0.1f; the old +/-0.0001f ended in 17. Kisak's smoothing
expression independently supports the product. Only the four coefficient
expressions changed; no ABI, new symbol or compiler flag change.
test_weapon_movement_literals.py extracts the actual loop, exercises ten
frame/prone cases with positive/negative steps and clamps, and checks linked
FMUL literal bytes. Lighting and assignment tests also pass. Numeric-only
gate is RATCHET OK, accepted, still 1,231,036 B: zero new code-byte credit.

Rejected attempts: removing the redundant outer movement loop lost two real
functions / 683 B; restored. Individual vector-copy batch (13 functions,
24 changes, two rounds) lost two real functions / 181 B in one round;
grouped-copy batch (13 functions, one round) gained no own matches. All
generated copy drafts restored. Logs: weapon_movement_literals_1011.log,
weapon_movement_numeric_only_1011.log, gated_copy_1011.log,
gated_copy_all_1011.log. No referee or tie classification changed.

audit_unmatched_literals.py is diagnostic only: 656 aligned literal sites,
four remaining rows after the correction. Three are known sign-fold cases;
nb_decode's row is a false alignment of commuted 0.95/0.05 products. Both
products are present with the correct operands; no Speex fix is claimed.
Instruction-run alignment alone does not justify changing source data.

Owner cleared the renderer goal; CoD2MP_s.exe is the active target again.
Renderer checkpoint19 remains saved separately, with source/text evidence
staged in gfx for owner review, not committed or pushed.

Fresh client start: **1,230,754 B**. Latest fresh gated link: **1,231,036 B
(87.39% of .text)**, **+282 genuine new game-code bytes** and no earlier real
matches lost. The meter includes existing libraries; game-source matches
are 1,084,401 B. No new tie or library gain is claimed.

Kept clean source in target/src:

- cg_ents_mp.cpp: CG_LockLightingOrigin, retail 0x4c5cc0, 170 B. Reuse
  existing EntVectorCopy for both origin copies, then set the render flag.
  Moving the flag alone worsened 2 to 10 diff lines and was superseded.
- scr_compiler.cpp: EmitAssignmentStatement, retail 0x46d3f0, 112 B.
  Test the unsigned byte of the predicate result without changing its int
  callee ABI. Matches retail sete/movzx/test al and all five call bindings.

Tests: test_lighting_origin.py exercises the actual helper with 32 cache/flag
fixtures; test_assignment_byte.py exercises actual predicates/assignment
branch with 8192 AST/clear fixtures and checks all five linked call targets.
The prior capture, compass, statics, shellshock, screen-storage, sound,
literal, client-init and call-binding tests were rerun for this source state.

Two bounded gated batches gave no further gain: distance2/3 swaps over
101 functions, then full-match-only split forms over 84 functions. All
generated changes were restored. One swap round lost 24 real functions /
7523 B and was rejected; one split round failed compilation and was restored.
Logs: client_resume_1011.log, gated_swap_1011.log, lighting_copy_1011.log,
lighting_vector_1011.log, assignment_byte_test_1011.log, gated_split_1011.log.
gated_swap.py reuses existing generators but forces the mandatory build and
ratchet, with split mode keeping only zero-diff forms. It changes no referee.

These are fresh linked function/regression results, not a new all-253-TU cold
replay, whole-image identity or Windows smoke claim. Existing vary.py work
is untouched. The two source changes are uncommitted pending a larger
checkpoint/review; no binary, release-monorepo change or push.

## Fresh client-first cold checkpoint - 2026-10-11

The earlier warm-only caveat is superseded here. All 253 source objects were
rebuilt from a fresh build directory containing only comparison baselines.
The old build is recoverably preserved in pre_cold_build_client_first_1011/.
Toolchain image remains sha256:17ddd3cc30517bc070c4e1a1d8bec9c4abd0d8b453115dfa0324e7e20dc94131.
Cold ledger is byte-identical to the incremental ledger:
0217a895e6c3f35f228c6b2b28c4cc60db782e6c70597cdb206aa076363f1e5e.
Fresh RATCHET OK, 1,231,036 B / 1,408,683 = 87.39%; game source 1,084,401 B,
MSVC libraries 93,735 B, Speex 52,900 B. Only the two previously described
functions add 282 genuine new code bytes. Scope trials add zero and are restored.

All twelve suites pass on the cold build: capture, compass, statics, shock
table, screen storage, sound selection, retail constants, client init, call
bindings, lighting origin, assignment byte and weapon movement literals.
Source/tool hashes pass after rebuilding and testing. Evidence:
client_first_cold_1011.log, client_first_tests_1011.log,
client_first_inputs_1011.sha256 and gated_scope_1011.log.

Release link succeeds with no LNK2001/LNK2019. SHA-256:
72c8a1372a41fd16d49f460b2ce623b2e57e5dd6f954740d6aaf147448325d55;
retail 8caf434c9509020b9c94720d0407a31d61411312996dab9964f46865f070900e.
These differ: V4 remains unmet; no Windows gameplay test (V5) was performed.
The code-byte meter is not complete source/data/callee verification.
Clean source remains target/src; no new commit or push, and unrelated vary.py
changes are preserved. Renderer checkpoint19 remains separately staged, not
committed: the owner must review and commit it before their push.
