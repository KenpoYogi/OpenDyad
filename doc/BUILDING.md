# Building and running

This tree includes the complete upstream extlib v82 delivery, as
[recovered and archived by OpenCourant](https://github.com/OpenCourant/extlib/releases/tag/upstream-v82-extlib):
the public libraries, the H3D interfaces and the matching input readers for
linux64, linuxa64 and win64. It builds without downloading external libraries,
and no legacy reader compatibility options are needed. The package URL and
checksum are recorded in `EXTLIB_VERSION.json`. See the
[validation record](BUILD_VALIDATION.md) for compilation and solver checks.

One limitation remains: `/ALE/STRUCTURED_MESH` is rejected by Starter with an
explicit error, because the readers' mesh generator produced degenerate meshes
(zero node coordinates, zero-volume elements) in every trial on both platforms.
ALE analyses on an explicitly defined mesh are unaffected.

Use the [current source ZIP](https://github.com/KenpoYogi/OpenDyad/archive/refs/heads/main.zip)
or clone `main`. The preserved `upstream-20260929` release predates the dependency
recovery. Extract or clone into a short path without spaces; some upstream
compiler flags do not support paths containing spaces.

## Supported build configurations

| Platform | Configuration |
| --- | --- |
| Linux x86-64 / WSL | GCC/GFortran, double precision, SMP |
| Windows x86-64 | Intel oneAPI, double precision, SMP |

The configurations above are the minimal SMP builds. The default
full builds, `build_windows_all.bat` and `build_linux_all.sh`, also produce the
MPI Engines with the MUMPS implicit solver, the single-precision executables,
the output converters and the staged GUI; they are described in
[HOWTO_IMPLICIT.md](../HOWTO_IMPLICIT.md). Other compiler families are outside
these build instructions; see [HOWTO.md](../HOWTO.md) for the upstream build
options.

## Linux and WSL

Install GCC, G++, GFortran, CMake, Make and Python 3. On Ubuntu:

```bash
sudo apt-get install build-essential gfortran cmake python3
git clone https://github.com/KenpoYogi/OpenDyad.git
cd OpenDyad
bash build_linux.sh 8
```

The optional argument is the number of parallel build jobs. In WSL, building in
the Linux filesystem is usually faster than building on a mounted Windows drive.
The executables are `exec/starter_linux64_gf` and `exec/engine_linux64_gf`.

`bash build_linux_all.sh 8` is the full build: with OpenMPI 4 installed it adds
the GNU + OpenMPI Engine with MUMPS, the single-precision executables and the
converters (see [HOWTO_IMPLICIT.md](../HOWTO_IMPLICIT.md)). Pass `-smp` to
include `engine_linux64_gf` as well.

Set the runtime paths from the repository root:

```bash
export OPENRADIOSS_PATH="$PWD"
export RAD_CFG_PATH="$OPENRADIOSS_PATH/hm_cfg_files"
export RAD_H3D_PATH="$OPENRADIOSS_PATH/extlib/h3d/lib/linux64"
export LD_LIBRARY_PATH="$OPENRADIOSS_PATH/extlib/hm_reader/linux64:$RAD_H3D_PATH${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
export OMP_STACKSIZE=400m
```

Copy the bundled twisted-beam example to a working directory and run Starter,
then Engine:

```bash
mkdir -p "$OPENRADIOSS_PATH/work/twisted_beam"
cp "$OPENRADIOSS_PATH/qa-tests/miniqa/SMOKE_TEST/data/"* \
   "$OPENRADIOSS_PATH/work/twisted_beam/"
cd "$OPENRADIOSS_PATH/work/twisted_beam"
"$OPENRADIOSS_PATH/exec/starter_linux64_gf" -i TWISBEAM_0000.rad -np 1
"$OPENRADIOSS_PATH/exec/engine_linux64_gf" -i TWISBEAM_0001.rad -nt 1
```

## Windows

Install Visual Studio C++ Build Tools with the MSVC toolchain and a Windows SDK,
then Intel oneAPI with the Fortran/C/C++ compilers and MKL. The verified setup
uses Visual Studio 2022 and oneAPI 2023.0.0. Use a oneAPI command prompt with those
components enabled, plus CMake, Ninja and Python 3 on `PATH`.

For a ZIP download, no Git checkout is needed. Run from the extracted repository
root (for example `C:\OpenRadioss`):

```bat
build_windows_compat.bat 8
```

This creates `exec\starter_win64.exe` and `exec\engine_win64.exe`, and copies
the input-reader DLLs (`hm_reader_win64.dll` and `libapr-1.dll`) beside the
executables. `build_windows_all.bat 8` is the
full build: from a plain command prompt it loads oneAPI itself and adds the
Intel MPI Engine with MUMPS, the single-precision executables and the
converters (see [HOWTO_IMPLICIT.md](../HOWTO_IMPLICIT.md)). Pass `-smp` to
include `engine_win64.exe` as well. Use the same oneAPI environment
when running, so that Intel compiler and MKL runtime DLLs are available:

```bat
set "OPENRADIOSS_PATH=%CD%"
set "RAD_CFG_PATH=%OPENRADIOSS_PATH%\hm_cfg_files"
set "RAD_H3D_PATH=%OPENRADIOSS_PATH%\extlib\h3d\lib\win64"
set "OMP_STACKSIZE=400m"
mkdir work\twisted_beam
copy qa-tests\miniqa\SMOKE_TEST\data\* work\twisted_beam\
cd work\twisted_beam
"%OPENRADIOSS_PATH%\exec\starter_win64.exe" -i TWISBEAM_0000.rad -np 1
"%OPENRADIOSS_PATH%\exec\engine_win64.exe" -i TWISBEAM_0001.rad -nt 1
```

## Dependencies and compatibility options

All dependencies come from the upstream extlib v82 delivery, archived by
OpenCourant as
[`upstream-v82-extlib`](https://github.com/OpenCourant/extlib/releases/tag/upstream-v82-extlib)
(git history on the
[`upstream-v82`](https://github.com/OpenCourant/extlib/tree/upstream-v82) branch).
They include LAPACK, METIS, zlib, MD5, Boost, ExprTk, the H3D headers and
libraries, and the input readers (`hm_reader`, build `20260918_f907fe76`) for
all three platforms. Third-party notices are under `extlib/license/`.

The readers export every interface this Starter calls, so the three
`HM_READER_*` compatibility options in `starter/CMakeLists.txt` default to
`OFF` and the helper scripts pass them as `OFF` explicitly. For direct CMake
builds:

| Option | Default |
| --- | --- |
| `USE_OPEN_READER` | `0` |
| `HM_READER_LEGACY_INCLUDE_API` | `OFF` |
| `HM_READER_LEGACY_PART_API` | `OFF` |
| `HM_READER_NO_STRUCTURED_ALE` | `ON` |

Turn a legacy option `ON` only when building against an older reader that
lacks the corresponding interface (`cpp_get_include_file_by_index`,
`cpp_is_part_with_elements`). `HM_READER_NO_STRUCTURED_ALE` stays `ON`, as in
OpenCourant: the v82 readers export `cpp_sale_mesh_create`, but with every
`/ALE/STRUCTURED_MESH/CONTROL_POINTS` syntax tried, the generated nodes had
zero or repeated coordinates and Starter reported zero-volume elements, on
Linux and Windows alike. Set it to `OFF` only to experiment with the
generator. The included OpenReader source does not yet provide every
interface required by this Starter and is not selected by these scripts.

### Replacing the input reader

To use a different reader build, replace the files under `extlib/hm_reader/`
and update the manifest entries in `EXTLIB_VERSION.json`, then build in a
fresh directory. On Windows, the build scripts copy `hm_reader_win64.dll` and
`libapr-1.dll` into `exec`; the import library supplied with the DLL is used
at link time. The loader only checks local files; changing the manifest does
not download a package.

## Regression checks

With the Linux runtime paths above set, configure and run selected bundled MiniQA
cases directly through CTest so its exit status is preserved:

```bash
cd "$OPENRADIOSS_PATH"
cmake -S qa-tests -B qa-tests/ctest_suite_linux \
  -Darch=linux64_gf -DPREC=dp -DMPI=smp -DNP=1 -DNT=1 \
  -DSTDOUT=0 -DKEEP=1 -DDEBUG=optimized -Dtype=default -Dpon_run=4x1,1x4
ctest --test-dir qa-tests/ctest_suite_linux -C Release \
  --output-on-failure --timeout 600 \
  -R '^1\.(01|03|04|05|10|17|64|68|84)$'
```

The existing runner checks Engine termination and compares results with each
case's stored references and tolerances. Do not treat a successful compilation
or `-help` invocation as a solver regression result.
