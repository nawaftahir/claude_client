# Next session prompt (paste this)

Continue the CoD2MP_s.exe 1.0 client recon in dev/claude_client (HEAD 5190c32, 77.05% = 1,085,427 B measured).
Load the client-recon skill and read LESSONS.md tail first. SPEED IS THE PRIORITY: ≥1% per 30–60 min.
- Solo, no agents. Cheap wins first: rescore (`python3 ../alldiff.py > $SP/ad.txt` from target/), then
  `python3 ../frame.py $SP/ad.txt` and `../andesp.py`, then ≤12-line near-misses.
- Time-box each function to ~3 builds / 10 min; park + log in LESSONS.md, move on.
- Keep an edit only if its own fn improves and the total doesn't regress. Real C/C++ only.
- Commit at every whole +1% (next ≥78.01% ≈ 1,098,870 B), `git -c user.name=Nawaf`, no Co-Authored-By,
  then `cp target/build/cmp.json target/build/cmp_base.json`. Report the measured % every ~30 min.
