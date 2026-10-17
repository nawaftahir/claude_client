# Handoff: CoD2MP_s.exe 1.0 client, Gemini trial, 2026-10-18

Goal: byte-match more functions of the CoD2 1.0 MP client in real C/C++. Claude reviews and merges your patches into main.

## State
- Main (`claude_client/`, branch master, commit after df36b7f): 1,269,196 / 1,408,683 unique `.text` bytes = 90.10%.
- Our build boots to the main menu and loads maps (10-17), so keep it runnable.
- Next milestone 91% = 1,281,902 B (+12,706 B).

## Rules (hard)
- Real C/C++ only. No inline asm, `_emit`, byte arrays, post-link patching.
- Never invent names, offsets or constants. Confirm from retail asm or a reference, otherwise skip.
- NO python sweep scripts (auto-variant generators). They paid 0 of 80 on these leftovers. Do the RE by reading.
- Do NOT edit `claude_client/` (main), `deepseek/`, `gfx/`, `radiant/`. Never `pkill`/`killall`, never `git checkout`/`reset` src.
- Max 3 builds per function, then write one line and move on.

## Your workspace
- Slot `dev/lab/s4` (warm worktree). First run `cd dev/lab && sh rebase.sh s4` to sync it to main (rebuild ~1 min).
- Everything else in `dev/lab/agent_common.txt` applies (fdiff, rasm.sh, smeasure.sh, tie list, paid fix patterns). Replace SLOT with `s4`.
- The docker image lost `dsound.lib`; `s4/target/dsound.lib` is already copied in. Leave it (untracked).

## Picking targets
1. From `dev/lab/s4/target`: `python3 ../../../claude_client/alldiff.py > ../scored.txt` (row: diff-lines size retail-addr mangled). Smallest diff lines first.
2. For each target grep its name in `dev/deepseek/LESSONS.md` and `dev/lab/w4/re_re_*.md`. If an attempt is listed, do not repeat it; find a new cause or skip.
3. Read the whole retail function (`claude_client/rasm.sh START END`) next to our source. Map every retail register and stack slot to a source variable before editing.
4. References: Mac 1.3 decompile with real names/types `dev/refrences/cod2-mp-macho/`, KisakCOD (shape), CoD2rev `test/Refrences/CoD2rev_Server/src`, SHA-verified server source `dev/cod2engine/archive/lnxded-1.0/src`.
5. Lever list that paid recently: `~/.claude/skills/client-recon/SKILL.md`, section "Fix-pattern catalogue" (plain file, read it).

## Deliver
- `cd dev/lab/s4 && git diff > ../gemini_s4.patch` (kept edits only).
- One line per function tried in `dev/lab/gemini_s4.md`: diff lines before -> after, what fixed it or what failed.
- Keep an edit only if your function's diff lines drop and smeasure shows no other real function lost (ties ignored).

## Measured baseline for comparison
Claude Opus agents, 45-min box, same slots: 2-6 bytes per 1k tokens on these leftovers (10-15 round: ~400k tokens, +1,452 B, 3 full matches). Report your token use so we can compare.
