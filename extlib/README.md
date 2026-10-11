# Build dependencies

This directory is the official upstream OpenRadioss_extlib v82 delivery
(2026-09-22), as recovered and archived by OpenCourant:

- package: https://github.com/OpenCourant/extlib/releases/tag/upstream-v82-extlib
- git history: https://github.com/OpenCourant/extlib/tree/upstream-v82

It is committed to this repository in full, so a clone builds without
downloading anything. `../EXTLIB_VERSION.json` records the package URL and
SHA-256 for anyone who wants to verify it against the archive.

The input readers under `hm_reader/` are the matching v82 build
(`20260918_f907fe76`) for linux64, linuxa64 and win64. They export every
interface this Starter calls, so the legacy `HM_READER_*` options in
`starter/CMakeLists.txt` default to OFF. `HM_READER_NO_STRUCTURED_ALE` stays
ON: the readers' `/ALE/STRUCTURED_MESH` generator produced degenerate meshes
in every trial on both platforms, so Starter rejects the keyword explicitly
(OpenCourant ships the same setting). On Windows the reader also needs
`libapr-1.dll` from the same folder; the build scripts copy both DLLs next
to the executables.

Third-party notices are under `license/`.
