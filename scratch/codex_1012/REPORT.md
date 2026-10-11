# Client correctness checkpoint - 2026-10-11

Review branch: `codex/1012`, source base `e30122f`. Renderer work stays paused.
This is reconstructed source repair, not game bug patching or a finished release.

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
