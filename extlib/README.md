# Build dependencies

The common libraries are taken from the final upstream OpenRadioss_extlib
delivery, v82, commit `f2fa2cd00dd5937d96e521f163a191f2dee90148`
(2026-09-22), whose git history is preserved on the `upstream-v82` branch
of https://github.com/OpenCourant/extlib. Compared to the earlier
v67-compat package this restores the rebuilt H3D writer libraries
(linux64/linuxa64/win64), the updated H3D headers including
`h3dpublic_import.h`, and the updated license texts.

Input readers were recovered separately from public OpenRadioss runtime
packages; matching v82 reader binaries have not been recovered.
`../EXTLIB_VERSION.json` records their sources and build IDs:

| Reader | Build ID | Effective extlib era |
|---|---|---|
| `hm_reader/linux64/libhm_reader_linux64.so` | `20260602_fcb3b29a` | ~v60 |
| `hm_reader/win64/hm_reader_win64.dll` | `20260710_d899773e` | ~v70 |

Original reader and third-party notices are under each platform's
`hm_reader/<platform>/notices` directory. Other libraries retain their
original notices in their own directories.

This collection is not the original v82 archive. The compatibility builds
reject `/ALE/STRUCTURED_MESH`: the Linux reader lacks the generator, and a
Windows trial with the v70 reader produced invalid node coordinates.
