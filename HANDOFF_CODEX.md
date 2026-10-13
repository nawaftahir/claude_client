# Handoff: CoD2MP_s.exe 1.0 client, 2026-10-12

## Return to Claude - checkpoint 2026-10-13

STOPPED AT OWNER'S REQUEST AFTER VERIFICATION. This section supersedes the
older queues below. No new matched bytes in this continuation. The retained
change improves readable source, not the percentage. Do not present it as
progress toward a new coverage milestone.

Accepted meter: 1,234,034 / 1,408,683 unique .text bytes = 87.60%, 7,051
matched functions / 7,027 unique out of 7,346. Breakdown: game source
1,087,399 B (77.19% of total .text), MSVC libraries 93,735 B (6.65%),
Speex 52,900 B (3.76%). The meter includes 668 name-paired short functions
(9,505 B); it is not a whole-file or full-source-completion percentage.
Both accepted good.json and fresh exp.json SHA-256 are:
fb381f5f5e7dd5bd3a7d64372a4c9c4a46af2c9c27e5272eed10c199a52ebe22

Kept source: target/src/oc/cgame_mp/cg_players_mp.cpp, SHA-256
a513f47e75ba98a9c536c68cf87130c8c722b9f4b72fb786dd0953cb46018384.
CG_Player/CG_Corpse now use the existing GfxEntity record instead of a
116-byte array and field casts. Player/Corpse/UpdatePlayerDObj use actual
clientInfo_t pointers and array indexing. Player reads nextSnap->ps.pm_flags
instead of byte-offset punning. RT_DOBJ replaces an enum cast of zero.
No control flow, null handling, ABI or record declaration changed.
Retail pm_flags is snapshot+0x18 (ps at 12, pm_flags at 12), NOT snapFlags.
Retained matches: Player 365 B at 0x4d61e0; Corpse 340 B at 0x4d6350;
UpdatePlayerDObj 91 B at 0x4d64b0. These 796 B were already credited.

Verification completed, then ../lab/mym.sh accept:
- player_typed_records_1013.log: RATCHET OK, identical complete ledger.
  Its 15-line diagnostic was run against WRONG address 0x4d61d0; ignore it.
- player_typed_records_fresh_1013.log: forced a fresh player TU build,
  correct address 0x4d61e0, difflines 0, RATCHET OK, same complete ledger.
- test_call_bindings.py, test_retail_constants.py,
  test_stream_save_layout.py and test_cgs_storage.py all pass freshly.
  Cgs fixture compiled/ran in the original MSVC image; 214 fields/12 calls.
- Five current manifests pass: cgs_checkpoint_inputs_1011,
  fx_shared_inputs_1013, xmodel_skin_inputs_1013, model_surface_inputs_1013,
  miles_audio_inputs_1013 (all under scratch/codex_1012, suffix .sha256).
  They cover prior retained work, not the newly typed player file.
No new player-specific layout/runtime fixture was added; the fresh full
referee reproduced all three existing matches and the complete ledger.

Bounded unsuccessful trials are RESTORED; do not retry as new work:
- Bullet_Fire_Extended moved to the verified Linux TU's source position:
  114 diffs, no gain. weapon_order_1013.log / weapon_order_restored_1013.log.
- FX_CalcOriginAndAxis inverted 0x40 branch arms, then reused x/y locals
  for polar calculations: 370 diffs each, no gain. fx_axis_branch_1013.log,
  fx_axis_shared_xy_1013.log. Shared FxUtil header work is preserved.
- BG_ParseCommands nested bodypart loop: 514 diffs instead of 209;
  body 1496 -> 1236 B, no gain. bgparse_nested_1013.log. Preserve retail's
  sizeof(pointer) four-byte command clear; it is not sizeof(*).
- Record_QueueAudioDataForEncoding cached/recomputed available samples:
  97 diffs instead of 57; adding the two-field counter group gives 105.
  audio_sample_counts_1013.log / audio_counts_group_1013.log. Both restored.
The final fresh player gate verifies the restored combined tree, not just
an isolated source draft. No partials, byte credits or ratchet exceptions.

Claude resume:
1. Work in this client tree, canonical target/src; renderer/release stay
   untouched. Read the current client-recon skill before editing.
2. Start from good.json and recompute the unmatched queue. The older
   handwritten queues below are historical: VM_ExecuteInternal already
   matches, and old gap lists contain multi-hit functions already credited.
   scratch/codex_1012/current_scored_1013.txt is a recent diagnostic snapshot,
   not a substitute for current referee hits or confirmed callee bindings.
3. Spend builds on genuinely unmatched source/caller-callee groups backed
   by target evidence, not more typed-only cleanup or new harnesses by
   default. Reuse the existing build/referee/tests. Two failed fresh variants
   per function, then park unless new binary evidence changes the hypothesis.
   Existing XAnim six-diff and audio grouped-counter drafts are partials,
   not untried wins. All recorded CPU flag trials are already exhausted.
4. Accept only fresh full matches, preserved protected matches and tested
   bindings. The owner's goal remains readable source AND whole-file identity.
   Whole-file SHA identity has NOT been achieved. No new gameplay test here;
   earlier Wine evidence is only local startup/map/connect/shutdown, not V5.

Dirty worktree deliberately retained: prior accepted source repairs, owner
tools (including vary.py/sweep.py), scratch and old artifacts coexist.
Do not bulk-reset, clean or commit everything. No commit or push at this
checkpoint; no retail-binary, asset, renderer or release-tree changes.

## Active continuation - 2026-10-13

Kept Miles import typing in snd_driver.cpp: WAV_info accepts const input
and AILSOUNDINFO *, size_processed accepts unsigned rate/format and const
AILMIXINFO *, process accepts the mutable mix record and 32-bit integer
arguments/result. Wave parsing now passes &mixinfo.Info explicitly. Signatures
are SUPPORTED by Kisak's SDK header, not claimed recovered Windows provenance.
Do NOT import its newer 40-byte sound record: CoD2's existing record is 36 B,
without channel_mask. No new code-byte credit; loader remains unmatched.

miles_audio_types_1013.log / miles_audio_types_fresh_1013.log both RATCHET OK,
identical accepted fb381f5f... ledger. Actual-TU original-MSVC probe validates
three function-pointer types and 36-byte record offsets; the negative probe
rejects all three old signatures (C2440). This is compile-time proof, not
a running audio test. Fresh call test checks all three Miles import identities
against retail; constants/save/cgs tests pass. Five current input manifests
pass. Source remains in target/src; no renderer/release edits or commit/push.

New bounded trials are finished and restored: one/two array references in
delta quaternion continuity leave the same six XAnimLoadFile diffs; Speex
shared/selected error locals give 9/11 diffs. No partials promoted.
CPU tests used same-input FULL fresh builds before the Miles declaration
cleanup, not one-object flags. Compiler help states
default /GB (blended), not /G6. /G7 loses 3624 protected real functions and
gives only 281155 covered bytes; rejected. /G5 and /G6 produce identical
ledgers AND all five section byte streams, as does restored default /GB.
No CPU override remains; cold default gate reproduces fb381f5f... exactly.
Logs cpu_g*_full_build_1013.log / cpu_g*_gate_1013.log and saved G5/G6 images
are build-only artifacts. Do not retry processor flags as new source work.
When comparing ledgers, do not put duplicate static mangled names into a dict
and claim a gain: it hides one entry. The full G5/G6 ledgers are identical.

Current accepted ledger is fb381f5f5e7dd5bd3a7d64372a4c9c4a46af2c9c27e5272eed10c199a52ebe22,
still 1,234,034 / 1,408,683 B (87.60%). Surface pointer types are corrected:
XModelSurfs holds XSurface **, XModelGetSurfaces takes XSurface ***, and
native/active headers, loader, DObj temporary and direct CL_InitRef callback
agree. Native surface tag is now XSurface_s, matching the active client.
Retail 48eac9 stores returned pointers at stride 4; getter 48ef00 returns
the pointer array and its four part-bit words. No opaque callback cast.
This is real source typing, ZERO new matched-byte credit.

model_surface_types_1013.log and model_surface_types_fresh_1013.log both
RATCHET OK, getter diff 0. Original-MSVC /GL /LTCG fixture compiles/runs
with 20-byte container offsets 0/4, four LODs/three surface pointers and
256 guarded model copies. Call test proves exact 44-byte getter, CL_InitRef
callback store, and renderer-relative slots 96/97 in both wrappers and the
array loader, including wrong-target checks. ReadSurface wrapper has
multiple masked hits; the renderer-relative binding identifies 48ea60.
Fresh constant/save/cgs tests pass (214 cgs fields and 12 calls).
Four current input manifests pass; accepted with mym.sh accept. No new
runtime or whole-file SHA claim, no commit/push, renderer remains read-only.

Unfinished SWP batch is now finished (75 completed trial rounds, exit 0):
only four baseline matches, no new full matches. Do not restart this queue.
ROQ precise-float plus signed-bottom trial: 112 diffs, restored.
SND deltaX scalar and per-TU /G7 /G5: no new matches, restored.
StartWeaponAnim rate-pointer iterator loses protected ResetViewWeaponAnimTrees
(146 B); direct one-based indexing preserves coverage but leaves 24 diffs.
Both restored. Current SND/weapon source hashes match the earlier checkpoint,
and no CPU flags or ROQ pragmas remain. Do not repeat these source forms.

Previous accepted ledger was 2290fec61ce54a073eb679e98abf6f5b3a72930ec5e695a312411153ce034627,
still 1,234,034 B / 87.60%. JSON changes are typed skin-getter/wrapper names,
NOT added coverage. Native XModel.xskins, loader export/wrapper, getter,
compat import declaration and CL_InitRef now agree on trXSkin_t pointers;
the getter returns const trXSkin_t *. The renderer callback is assigned
directly, without the old void** cast. The active trXSkin_t.materialHandles
is MaterialHandle *, not a function pointer. Retail renderer 1002024c/4f
confirms LOD stride 4 then material-array stride 4; getter 48d410 reads +80.

First skin build failed because the old Material field implicitly declared
Material for two effects TUs. Added an explicit struct Material forward;
corrected/fresh logs xmodel_skin_types_corrected_1013.log and
xmodel_skin_types_fresh_1013.log both RATCHET OK, getter diff 0.
Original-MSVC fixture passes 144/100/4-byte model/parts/skin sizes, offsets,
256 guarded copies and four LODs/three materials. test_call_bindings.py
checks the exact 11-byte getter and its CL_InitRef store against retail,
including wrong-address rejection. Constant/save-record tests and all three
current manifests pass; accepted with mym.sh accept. No commit/push,
no renderer source edits, no new runtime or whole-file SHA claim.

Kept actual source-type repair: oc XModel.parts is XModelParts_s *, not
void (*)(). Native xanim_public.h, XModelLoadFile's pointer assignment and
retail loader/accessor dataflow agree. Both fresh original-compiler gates
xmodel_parts_type_1013.log / xmodel_parts_type_fresh_1013.log preserve the
accepted e73abe78 ledger. test_xmodel_parts_type.cpp compiled /GL /LTCG and
ran under Wine: model/parts sizes 144/100, parts/memUsage/skel offsets
0/132/20, typed pointer access, 256 copies and neighbor guards pass.
Call/constant/save-record regressions and updated manifests pass. Accepted
with mym.sh accept; ZERO new code-byte credit, no runtime bug claim.

Menu_Paint blur product staging gives 14 parent diffs and loses its protected
470-byte standalone body. Kisak's two-local sum/sqrt form preserves that
body but still gives 14 parent diffs. Neither is kept; source restored and
menu_blur_restored_1013.log passes the ratchet.

Shared FxHelper/FxCamera declarations now replace seven differing class copies
in target/src. Both main gates (fx_shared_main_1013.log and
fx_shared_main_fresh_1013.log) preserve the accepted ledger exactly. Original
MSVC layout fixture pins camera/helper sizes 112/252 and member offsets;
fresh call/constant/save-record and actual cgs storage tests pass. Makefile
normalizes MSVC's lowercase effectscore dependency paths; no-op make no longer
recompiles these TUs. This is source consistency, ZERO new code-byte credit.
Input manifests cgs_checkpoint_inputs_1011.sha256 and fx_shared_inputs_1013.sha256
both pass. No whole-file identity or new runtime claim.

Scr_GetNextCodepos per-caller inline_depth(0) trial gives 447 diff lines,
not a match; restored, next_codepos_restored_1013.log RATCHET OK.
Client_SendVoiceData early return reproduces the retail shared epilogue,
but still differs at memcpy source register/setup (4 diff lines). Reversing
its parameters plus every declaration/caller leaves the same four diffs.
Neither is accepted; all files restored, voice_restored_1013.log RATCHET OK.
Original-MSVC /GL /LTCG dvar fixture recompiled against the shared-header
tree and passed its layout/256-pattern runtime checks. No new bytes.
Historical restart of the now-finished SWP queue in lab/codex_1012_swap_kQinUk:
swap_complete_1013.log, session 83889, PID 3514, 80-round limit;
141 functions/1288 candidates, completed rounds 0-58 have no new full matches
(four listed matches already accepted). Earlier SWP stopped at
round 7, so its remaining queue was not exhausted. Source rebase agrees with
canonical (core dumps excluded); private results remain unaccepted.

Accepted baseline remains 1,234,034 / 1,408,683 B (87.60%). No new full
match accepted in this continuation. Original 10-12 SWP/CACHESW parents
were interrupted at the owner's pause, exit 130; neither queue was
exhausted. No surviving driver was found at resume. Private source
snapshots are preserved as paused_snapshot_1013 in both owned lab trees.

The full split-assignment queue finished in lab/codex_1012_swap_kQinUk,
splits_full_1013.log, session 71942 exit 0: 84 functions, 434 initial
candidates, 421 valid checks across 40 rounds. NO new full matches;
the three listed matches were present at baseline and earn no credit.
Private partials remain for evidence, not accepted source. Main Item_Paint width
stage gives 38 diffs; adding a named actual angle and staging final Y gives
32 (includes jump-table data). Neither matches. Draft/source diff saved
as scratch/codex_1012/item_paint_partial_1013.*; canonical file restored.
item_paint_restored_1013.log is RATCHET OK; exp.json/good.json both SHA
e73abe784d5e6d9e071638396fa4ac21d5b1710b5f3efbd92140a3986694ec9b.

VectorRotate assignment/declaration/grouped-output trials failed and
are restored; rotation_domain_restored_1013.log is RATCHET OK. The
private cached-angle scorer's zero needs unused dAngle/empty statement;
it is NOT accepted or promoted as source. Explicit Dvar error-buffer
trial also failed and was reverted. Commutation duplicate was stopped
before trials: old 334-candidate/40-round evidence already exists.
No commit/push, renderer/release trees untouched, owner's vary.py preserved.

## Resumed toward 90% - 2026-10-12

Kept real source typing cleanup: active oc/headers/cod2_defs.h now uses
bool for DvarValue.enabled/dvar_s.modified and dvar_s pointers for next/
hashNext, agreeing with universal/dvar.h. Full rebuild ratchet passes;
accepted ledger is unchanged e73abe78... / 87.60%, NO code-byte credit.
test_dvar_types.cpp compiles against the actual active header with the
Makefile's original MSVC OCFLAGS and /LTCG; layout pins 4/8/36 B and
modified/current/next/hashNext offsets 7/8/28/32 pass. Wine runtime exits
0 after 256 legal boolean patterns, typed links, copy and guard checks.
Call-binding/constant/save-record regressions pass; input manifest now
has 21 entries. Logs: dvar_header_types_90.log,
dvar_types_fixture_built_90.log and dvar_types_fixture_run_90.log. No
whole-file identity or gameplay claim; no commit or push.

Owner revoked the pause and requested 90% verified coverage. Accepted
baseline remains 1,234,034 B; threshold is 1,267,815 B, requiring 33,781 B.
Never count restored ties as fresh reconstruction. Isolated vector/type
experiments are in lab/codex_90_vectors_RLh96F, not canonical progress.
The checkpoint below is historical, not an instruction to remain paused.

Latest accepted state still equals e73abe78... / 87.60%. All canonical
source trials from this pass were restored; restored_after_dobj_90.log
is RATCHET OK with exp.json equal to good.json, nineteen input hashes
pass. DObj's partial for-loop/unsigned-flag patch is preserved separately
as scratch/codex_1012/dobj_model_loop_90.patch, NOT accepted source.

Fixed sweep.build's MSVC error filename parsing: normalize backslashes
before basename. Windows/POSIX failure and success fixture passes in
test_build_error_paths.py. This changes error isolation, NOT the matcher.
types90_fixed.log legitimately tested 56+45 signedness variants, with
three partial improvements and zero new full matches; all private
partials were restored. Earlier types90 logs aborted whole rounds due
to the parser defect and do not establish source-level failure of the
valid variants. vector_ops90.log tests only arithmetic vector triples
(plain copies excluded): 8+3 valid trials, zero improvements, restored.

Completed cache_reads90.log: 23+10 trials, no improvements/new matches.
Completed scope_hoist90.log: 8+5 trials (Pickup_Weapon excluded), no
improvements/new matches. Both private trees restored. FX origin height
staging and origin-copy tests also failed; main restored with
restored_after_fx_origin_90.log RATCHET OK, exp/good hashes identical and
nineteen input hashes green. No canonical game-source change this pass.
The earlier two-round batches are terminal, but their candidate lists
were NOT exhausted: the drivers serialize functions sharing a TU. Full
typesw run completed after 24 rounds: 315 valid trials, five partial
reductions and NO new full matches. Snapshot: type_full_snapshot_90 in
lab/codex_90_vectors_RLh96F. Private tree rebased to current clean source;
CACHESW_WIDE full queue now runs there, cache_reads90_full.log, session
8335. Independent SWP full queue runs in lab/codex_1012_swap_kQinUk,
swap_full90_valid.log, session 8226: 142 functions/1301 candidates,
currently a SND_SetChannelInfo partial (7 lines), not accepted. Do not
launch a second writer in either tree. Only main
verification can accept a new match; partial reductions are not progress.
Additional bounded tests: CG_AddPlayerWeapon restoring its unused
weapInfo pointer gives 149 diff lines; positive outer guard gives 128,
neither a match. Both restored; player_weapon_restored_90.log is RATCHET
OK with exp/good SHA e73abe78... unchanged. Sys_IsLANAddress function-local
global-optimization-off worsens to 163 lines and loses CL_ReadyToSendPacket
222 B (ratchet rejects); pragma removed. Do not repeat these forms.
Client_SendVoiceData early return and SND_RawSamples staged writePointer
also do not match, both restored. UI_Init private dvar lookup/register/
setter chain grows to 1642 B vs retail 1546, frame 0x414 vs 0x40c; five
protected functions lose 354 B. Rejected, originals restored; draft copies
and ui_lookup_chain_built_90.log remain in scratch/codex_1012. No source
or percent credit for these attempts. Restoration gate is
ui_lookup_restored_90.log; wait for its result before any next main edit.
Keep only genuinely new full matches, reapply through apply_patch, then
main mym ratchet/bindings/tests before accept. Do not count already-
accepted zero-diff rows or ties. Ninety percent remains unachieved.
Owner's vary.py and all prior uncommitted accepted source are preserved.

## Accepted prefetch checkpoint - paused at owner's request, 2026-10-12

Accepted ledger: 1,234,034 / 1,408,683 unique .text bytes (87.60%).
This checkpoint adds 59 genuinely new bytes and restores a known 572-byte
Scr_ParseGameTypeList tie. The net +631 is NOT +631 reconstructed bytes.
good.json SHA-256: e73abe784d5e6d9e071638396fa4ac21d5b1710b5f3efbd92140a3986694ec9b.

Clean source is target/src/universal/com_shared.cpp. Com_Prefetch extracts
the existing handwritten prefetch loop from Com_Memcpy into an enum/switch
helper; Com_Memcpy calls it with PRE_READ. Retail helper 0x42c330 matches
all 59 raw bytes without masking. The existing 295-byte Com_Memcpy match
at 0x42c0b0 is preserved. Account the helper separately as C++ plus reused
source assembly, NOT pure C++ reconstruction or proven historical source.
KisakCOD/src/qcommon/common.cpp supplies the supported name/enum/switch
reference; retail instructions are the authority.

prefetch_extract_1012.log and prefetch_fresh_1012.log are fresh RATCHET OK
builds with no protected losses. test_prefetch_extract.py compiles the
actual source with original MSVC /O2 /GL and /LTCG: unmasked helper bytes,
guarded prefetch reads/no writes, and 4108 guarded Com_Memcpy cases pass.
Current call-binding, retail-constant and cgs storage/field/branch tests
also pass; storage log is prefetch_storage_regression_1012.log. Evidence
and the nineteen-input hash manifest live in scratch/codex_1012/.

No commit or push: genuine gain is below the whole +1% commit policy.
Existing uncommitted work and owner's vary.py are preserved. Goal paused
after this checkpoint; do not start new batches until the owner resumes.
Whole-file SHA identity and full gameplay remain unfinished. No new
runtime smoke was run for this helper extraction; prior frozen smoke
applies only to its documented image. Reuse the existing referee, avoid
repeating the reverted source trials below, and do not count tie flips.

## Local-grouping pass - 2026-10-12

No new matches; both source files restored. Record_QueueAudioDataForEncoding
two-counter aggregate (bytesEncoded, frameSize) fixes their reversed stack
slots, 57 -> 39 diff lines; adding partialSamples worsens to 81. UI game-type
cursor (index, parse, count) gives 39 vs 34; grouping only index/count is
unchanged at 34. Logs: record_grouped_locals_1012.log,
record_grouped_three_1012.log, gametypes_grouped_cursor_1012.log and
gametypes_grouped_counts_1012.log in scratch/codex_1012. No partial credit.
grouped_locals_restored_1012.log passes the unchanged ratchet, exp/good
hashes both b50a9658..., seventeen checkpoint hashes pass. The typed sound
save cleanup below is still kept; no commit or push, goal remains active.
For audio, investigate the remaining partial-sample memcpy/value lifetime,
not another declaration permutation; Mac STABS names samples_copied and
samples_in_our_buffer separately. This is a reference lead, not retail proof.

## Typed sound-save source checkpoint - 2026-10-12

Kept a source-only reconstruction cleanup in target/src/oc/snd.cpp:
SND_RestoreStreamChannel now uses snd_save_stream_t instead of char[32]
with float/int pointer casts and numeric origin offsets. Uses existing
fields fraction/rate/basevolume/org; passes &info to its callees and
sizeof(info) to MemFile_ReadData. Six offset assertions and the 32-byte
size assertion compile under the original MSVC 7.1. No new structure,
helper function, flag, behavior, assembly or matching rule.

Both typed_stream_info_1012.log and typed_stream_final_1012.log are
RATCHET OK. Compared the final fresh output to the frozen pre-edit
generated image before_typed_stream_1012.exe: all five section names,
virtual addresses, virtual sizes and raw bytes are identical, including
the entire 1,413,120-byte raw .text section. The complete accepted ledger
is identical (b50a9658...). Generated images stay uncommitted.
test_stream_save_layout.py uses the actual active declaration from
headers/PC/universal/com_math.h, not the inactive common_types.h dump;
256 raw patterns check all eight serialized words and a bit-exact round
trip. The MSVC compile pins the original layout; the native fixture
checks field access/serialization, not the full restore function.
Seventeen checkpoint input hashes now pass. Previous seventeen-suite
results apply to the identical generated section bytes; not rerun here.

Coverage is still 1,233,403 B / 87.56%, NOT a new full match for the
782-byte stream restore body. This is readable source progress only.
No commit or push. Goal remains active; whole-file identity/full gameplay
are still unfinished. Historical SetStreamChannelFromSaveInfo convention
notes are stale: the current 30-byte callee already matches 0x4b4310.

## Resumed source pass - 2026-10-12

Goal is active again. No new accepted bytes this pass. Full restoration
source_resume_restored_1012.log is RATCHET OK at 1,233,403 B; exp/good
JSON hashes are identical to the accepted b50a9658... ledger, all fifteen
checkpoint input hashes pass, and git diff --check is clean.

Bounded source tests, all reverted: PM_UpdateViewAngles constant-left
positive ladder comparison gives 17 vs 6 diff lines; compass split centerY
plus named radians gives 645 vs 902 but remains unmatched (frame still
0x9c vs retail 0xac). CG_DrawDevString named float height reproduces retail
early fild and improves 16 to 12 lines; staging the negative text width
adds no further improvement. SND_LoadFromBuffer byte procFormat worsens
52 to 73. Logs are respectively ladder_positive_left_1012.log,
compass_radians_split_1012.log, devstring_height_float_1012.log,
devstring_staged_width_1012.log and sound_buffer_format_byte_1012.log
under scratch/codex_1012. Do not describe any partial as accepted coverage.
No new tool, compiler flag, assembly, metric change, commit or push.

## Paused checkpoint - 2026-10-12

Owner requested a quick checkpoint and pause. Accepted code remains
1,233,403 / 1,408,683 bytes (87.56%); no new accepted code this continuation.
Clean source remains in target/src. Fresh restoration build
scratch/codex_1012/tracer_restore_gate_1012.log reports RATCHET OK;
exp.json and good.json both hash to b50a965837f2f76cff1cb0337ea16dded511e46fccc2cd9b71d6cf2b7fd0f8cf.
All fifteen cgs_checkpoint_inputs_1011.sha256 entries pass. The previous
seventeen-suite result remains applicable to unchanged accepted source;
those suites were not rerun. No commit or push; preserve owner vary.py.

Fully reverted source experiments, do not repeat without new evidence:
BG_CheckProneValid explicit VectorMA expansion and staged XYZ outputs
both worsen 12 to 14 diff lines. ScaleTracer early return worsens 54 to
56. CalcTracerFinalScale float-to-double return also gives 56 and loses
its protected 69-byte match. Logs: prone_ma_source_gate_1012.log,
prone_ma_staging_gate_1012.log, tracer_early_return_gate_1012.log and
tracer_return_type_gate_1012.log under scratch/codex_1012.

Resume with source reconstruction, not more release/runtime tooling.
Mac STABS return-type triage was read-only and produced no accepted change;
inspect actual retail return width/callers before changing types. Check
old notes against good.json: Scr_ResetTimeout already matches 15 bytes
and VM_ExecuteInternal already matches 10,812 bytes, so the historical
rdtsc-blocker note is obsolete. No new assembly was written.
Release-profile and local runtime evidence below is retained, not fresh
code credit. Whole-file SHA identity and movement/combat remain unverified.

## Release-profile repair - 2026-10-12

target/Makefile release now uses /DEBUG and the retail PDB pathname,
c:\builds\crc6_v1\pc\cod2\pc\CoD2MP_s.pdb, as well as /FIXED.
Retail has one 66-byte RSDS CodeView record; the old release omitted it.
The linker generates this record and a usable PDB; no retail bytes patched.
Release links fail on errors/unresolved externals, and Makefile changes
invalidate the release image. Fresh make release succeeds; the shape
fixture test_release_debug.py passes and rejects the non-release image.
make -C target check compares every release byte to the anchor and
correctly fails today (release_identity_gate_1012.log); it is the whole-file
gate, not the shape fixture or .text meter.
Independent fixed/debug links have identical full .text bytes. The /DEBUG
relocatable diagnostic's referee ledger equals good.json byte-for-byte.
Coverage remains 1,233,403 B / 87.56%; no code gain from this change.
Whole-file SHA still differs: timestamp/GUID, code and layout are unresolved.
Generated EXE/PDB stay uncommitted. Evidence: debug_link_1012/,
debug_release_1012/ and debug_release_final_1012.log under scratch/codex_1012.

## Runtime comparison - 2026-10-12

Actual retail AND rebuilt release initialize renderer/sound, load
mp_toujane and reach CS_ACTIVE with a private Codex smoke profile.
Rebuilt's loopback listener at 127.0.0.1:28972 and game window were observed
live. This verifies local startup/map initialization/connection, NOT
whole-file identity, remote multiplayer, movement/combat or full V5.
No source bug patch was made for Wine. Test setup needs the Miles plugins
and allowdupe; missing plugins faulted at the corresponding sound instruction
in both executables. Unknown snd_enable is not a disable switch.
Retail's map run timed out (124); rebuilt's first run returned 1 without a
crash trace after a live release relink. Do not assess shutdown from it.
The rebuilt retest uses a frozen generated EXE copy, not the live symlink.
That retest completes with exit 0, after CS_ACTIVE, quitting, CL_Shutdown,
Server Shutdown and ShutdownGame. Retained console:
wine_smoke_KrV6yt/rebuilt_profile_map_console_frozen.log. Frozen EXE SHA
402cfdff16b64f70da85d7363db94e7951738a89eff149aadba7aa3751d38353.
Retail's repeat also exits 0 with CS_ACTIVE and client/server shutdown;
console saved as original_profile_map_console_repeat.log. Neither final
run has an unhandled fault. Both private listeners are gone after exit.
See scratch/codex_1012/wine_smoke_KrV6yt/README.md for inputs/commands and
the retained console logs. Game assets are untouched; writes are isolated
in home_original/home_rebuilt. These are runtime evidence, not new bytes.
The fresh development gate release_profile_regression_1012.log is green
at 1,233,403 B and unchanged JSON b50a9658...; fifteen input hashes pass.
Archived three generated PDBs from the old cl_net_chan/cg_draw/g_weapon
/Zi probe outside target/src to scratch/codex_1012/compiler_probe_pdbs/.
Nothing deleted; no source edit, commit or push from that cleanup.

## Latest accepted state - stream source checkpoint, 2026-10-12

1,233,403 B / 87.56% matched .text: +1,089 genuine bytes over the
previous checkpoint, SND_StartAliasStreamOnChannel at 0x4b36f0.
Clean source: target/src/oc/win32/snd_driver.cpp. In the non-Q3-filesystem
branch, resolve FS_ShortOSFilePath into a local before AIL_open_stream.
This preserves behavior and fixes the argument scheduling; 326/326
instructions match. Selecting a shared filename before both branches
instead worsened seven diff lines to 171; that trial is fully removed.

Ratchet OK with no lost functions, all seventeen suites pass
(stream_path_regression_1012.log). The actual-source MSVC fixture covers
both filesystem branches and null/non-null stream handles. Binding checks
pin all nine direct calls and thirteen DLL imports; eleven manifest inputs
pass. Accepted JSON SHA:
b50a965837f2f76cff1cb0337ea16dded511e46fccc2cd9b71d6cf2b7fd0f8cf.
No compiler flag, byte mask, metric, helper or tie exemption changed.
No commit or push; vary.py remains owner-owned. Whole-file identity and
boot/connect/play remain unverified. Do not repeat the previous broad,
low-yield searches: reuse this path-first source pattern where evidenced,
otherwise use the existing ranked candidates and two-build limit.

Further clean-source repair, same 87.56% meter: cl_cgame_mp.cpp now has
eight readable g_color_table entries, not 36 entries padded with unrelated
Mac memory and replaced NaN bits. CL_LookupColor only indexes its first
eight rows; the bound 128 bytes equal retail at 0x55bfb8. Removed 448 bytes
of unused initializers, not counted as reconstructed code. The actual
lookup and ColorIndex source pass all 256 byte inputs with distinct team
colours and output guards. Eight linked team-colour reads and actual
MSVC offsets agree: allies +0x862c, axis +0x863c from clients; retail
CL_ClearState pins its base 0x8dc208. All seventeen suites pass
(color_final_regression_1012.log), thirteen manifest inputs pass, and
color_final_gate_1012.log is green. JSON SHA remains b50a9658... above.
Damage-indicator local-scope trial leaves the same stack-slot differences;
early cached weaponConfig in CG_ConfigStringModified also leaves code
unchanged. Both fully restored. The two-round existing swp.py batch in
lab/codex_1012_swap_kQinUk is finished: 95 swaps, no improved functions,
no new matches, source trees identical at completion. Its three listed
zero-diff bodies were already matched (chat, bullet, stream). Nothing
imported; do not rerun these same initial swaps as fresh work.

Latest continuation: no new accepted code. XAnimCalc precision pragma,
VectorRotate output-slot/parameter variants and two nb_decode expression
variants are fully restored. Reference-type batch: nine attempts, no
improvement. Braced split batch: 84 attempts, only two partial improvements,
no new matches; nothing imported. Restoration build
batch_restored_gate_1012.log passes the ratchet, current/accepted JSON
hashes agree, and all thirteen checkpoint inputs still pass. The previous
seventeen-suite results apply to the unchanged accepted source; they were
not rerun this continuation. Detailed failures are in deepseek/LESSONS.md.
Do not repeat these initial batches or describe them as coverage progress.

## Previous accepted state - 2026-10-12

Accepted 1,232,314 B / 87.48% matched .text; all seventeen suites pass
(source_batch_final_tests_1012.log). good.json SHA:
94a594a187fe42d87dedd023e0459c805f67adc33a57179ff783bd445cf0250a.
Only 48 genuine new bytes this continuation: SND_GetSampleType_4b30f0,
with unmasked byte equality and 5780 pinned-compiler input fixtures.
SND_StartAlias2DSample reuses that helper and retains its existing match.
Scr_ParseGameTypeList's known scheduling tie loses 572 measured bytes;
net meter change is -524, NOT a reconstruction gain of a percentage point.
CG_DamageFeedback now uses viewaxis[1] for roll and [0] for pitch,
not [2]/[1]. Six linked operand bindings and 27 camera fixtures pass.
These field repairs do not earn new code-byte credit. Clean source stays
in target/src; input manifest has ten passing entries. No commit or push.
Explicit shared run-equipment goto trial worsens 493 -> 578 diff lines;
fully restored. Fixture code bounds exclude validated embedded switch
tables from disassembly; no referee mask, coverage or tie rule changed.
Owner reports disproportionate quota cost: do not continue low-yield
near-miss experimentation or tool rewrites without reassessing the run.

## Typed client-state checkpoint - actual date 2026-10-11

Accepted **1,232,838 B / 87.52% matched .text**, **976 genuine new bytes**
over eeeb075: CG_DrawChatMessages 0x4c14d0/724 and CG_BulletHitEvent
0x4d18e0/252. Ratchet green, all seventeen test_*.py suites pass.
good.json SHA e7e9a521fd784963ffb30f877899a8006794c84f39cc73739e5dcfd6447db072.

Clean source: target/src/oc/cgame_mp/{cg_main_mp,cg_draw_mp}.cpp and
headers/PC/cgame_mp/cg_local.h. cgsArray is now cgs_t[1], not a one-byte
placeholder. Retail CG_Init clears 0x39f5 dwords at 0x136a800; the typed
object is asserted to be exactly 0xe7d4 bytes. Old linked storage put a
live dvar at +4 and cgArray at +16 inside that clear. The pinned-compiler
guard fixture checks the actual clear and adjacent guards, with asserts
ENABLED (unlike the production /DNDEBUG build).

Relocation masking initially hid the wrong chat material: retail reads
cgs+0xba28, the teamStatusBar slot, not whiteMaterial at +0xba1c.
Retail 0x4b9aac registers hudcolorbar and 0x4b9ac4 saves it to 0x1376228.
Fixed the source field; no cgMedia_t layout change. The new fixture checks
all eleven cgs references and twelve direct calls across the two matches.
Evidence: cgs_chat_binding_gate_1011.log, cgs_storage_test_1011.log,
cgs_all_tests_1011.log; fixture scratch/codex_1012/test_cgs_storage.py.
Whole-file identity and game boot remain unverified. vary.py belongs to
the owner; leave it untouched/uncommitted. No push.

Further data-binding repairs retain the same meter, not new byte credit:
cg_weapons.cpp writes hintMaterials[weaponNum+4] and falls back to [2]
(old +2/[0] were wrong); all 214 cgs memory references across unique
matches now agree. The actual source branch passes 254 pinned-compiler
icon/fallback cases and neighbor guards. CG_DrawFollow, CG_UseOffHand
and GetSortedHudElems use nextSnap (+0x24), not snap (+0x20).
CG_AddViewWeapon's early-out checks testGun (+0x2bfd8), not shellshock's
hasSavedScreen (+0x2ccf4). Tests pin the four corresponding retail
instruction sites and actual Windows struct offsets. Additional clean
files: cg_weapons.cpp, cg_hudelem_mp.cpp, offhandweapons_mp.cpp.
Final logs: next_snapshot_binding_gate_1011, view_weapon_binding_gate_1011,
cgs_final_tests_1011. No new helper, compiler flag, tie exemption or mask.
Owner priority saved in client-recon skill: source-first larger batches,
existing harness, at most two builds per candidate without new evidence;
do not spend sessions rewriting Python or recapping low-yield work.

Further source repair: cg_event_mp.cpp's bullet-hit cases 0xb5..0xb7 use
eventParm2 (+0xd8), not dmgFlags (+0xdc), for the reflected direction.
Retail 0x4daf93 and server G_RunMissile_CreateWaterSplash establish it;
the Mac/opencod2 exports repeat the wrong field. Existing storage fixture
now checks the actual linked ByteToDir call, both struct offsets, and
486 actual-source branch cases with distinct damage flags. Gate and
fixture pass (event_reflection_gate_1011 / event_reflection_test_1011).
All seventeen suites pass again (event_reflection_regression_1011.log).
The input manifest now includes cg_event_mp.cpp and the extended fixture;
all eight listed hashes pass. Accepted/current JSON hashes remain equal.
Meter unchanged: this is a real source repair, not a fully matched event
dispatcher. No new tool, metric change, flag, helper or tie exemption.
Counted WarpTime trial made no gain and is fully restored; restoration
gate passes. Do not retry that trial or misidentify 0x4317f0 as CRT code.

## Larger-group continuation - actual date 2026-10-11

Source checkpoint committed locally as eeeb075; no push. A subsequent bounded
continuation earns ZERO additional bytes: wrapping/negated delta products
remain six diffs; CG_PlayLoopedFx's existing Vec3DotInl form stays eight;
FloatSwap aggregate return worsens to 30; G_CallSpawn's explicit shared-exit
goto still has the wrong raw je target; IP-ban input caching worsens 4 -> 8.
All five drafts are restored. Do not repeat these forms. Latest restoration
proof: continuation_restored_gate_1011.log / continuation_restored_tests_1011.log.

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
restored; animation_6diff_1011.patch saves this lead against f5c3adb (also
contains the accepted signed-reader edits: never blindly apply). Its native
i386 actual-loader/types test passes 20 quaternion/flip fixtures and four
failures, but the function is NOT counted. The apparent protected
8-byte library _Copy EH loss was an out-of-text linker placeholder, not code.
The bounds correction below resolves that false regression. Operand
swapping and statement splitting did not help. Saved animation_14diff_1011.patch
(against f5c3adb, includes the accepted signed-reader edits) and .txt: inspect
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
Manifest model_checkpoint_inputs_1011.sha256 passes. Four changes vs the original
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
