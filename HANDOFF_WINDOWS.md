# Handoff: client recon, Windows session (10-18)

Target: `CoD2MP_s.exe` 1.0. End goal: every function real C/C++, whole exe
SHA-256 identical to retail, runs like retail. 91% (1,281,902 B of 1,408,683 B
`.text`) is only a checkpoint.

## State

- Main = this repo, `target/`. Measured **1,273,258 B = 90.386%** (`lab/mym.sh`,
  RATCHET OK, accepted as `target/build/good.json`). Checkpoint needs +8,644 B.
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
