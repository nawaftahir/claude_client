# Next session prompt (paste this)

Continue the CoD2MP_s.exe 1.0 client recon in dev/claude_client (91.01% = 1,282,075 B measured,
commit 6000f97). Load the client-recon skill, read HANDOFF_WINDOWS.md and the LESSONS.md tail first.
- Best measured route: 2 Opus RE agents in own rebased slots (lab/rebase.sh sN, run detached with
  nohup), brief lab/w4/prompt_w18_common.txt, 4-6 fresh fns each, gate with lab/gate_w18.sh <abs patch>.
  10-19 waves: ~330k tokens per wave, +200..+680 B.
- Apply the 10-19 levers (skill fix-pattern catalogue) to every fn with that diff shape.
- Commit at every whole +1% (next 92% = 1,295,989 B), `git -c user.name=Nawaf`, no Co-Authored-By,
  then `cp target/build/cmp.json target/build/cmp_base.json`. Report % and SHA distance (shameter.py).
