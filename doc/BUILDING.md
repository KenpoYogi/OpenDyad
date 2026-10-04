# Building and running

This tree includes OpenCourant's
[v82-hybrid dependencies](https://github.com/OpenCourant/extlib/releases/tag/v82-hybrid):
the recovered v82 public libraries and H3D interfaces, combined with the
previously recovered input readers. It builds without downloading external
libraries. This is not the original complete v82 distribution; reader compatibility
options are still required. Origins are recorded in `EXTLIB_VERSION.json`.
See the [validation record](BUILD_VALIDATION.md) for compilation and solver checks.

Use the [current source ZIP](https://github.com/lililii124/openradioss-261001/archive/refs/heads/main.zip)
or clone `main`. The preserved `upstream-20260929` release predates the dependency
recovery. Extract or clone into a short path without spaces; some upstream
compiler flags do not support paths containing spaces.

## Supported build configurations

| Platform | Configuration | Input reader limitation |
| --- | --- | --- |
| Linux x86-64 / WSL | GCC/GFortran, double precision, SMP | `/ALE/STRUCTURED_MESH` generation is unavailable; Starter stops with a diagnostic when this keyword is present |
| Windows x86-64 | Intel oneAPI, double precision, SMP | The compatibility build rejects `/ALE/STRUCTURED_MESH`; an automatic-generation trial produced invalid node coordinates |

Both compatibility builds print the includes retained in the loaded reader model.
For converted keyword decks, that list describes the converted model: filenames
can change and includes without converted entities may be absent. The list is
informational; it is not a complete record of the original input files.

This limitation concerns automatic structured mesh generation. It does not
disable ALE calculations on an existing mesh. An explicit 27-node, 8-element
fluid mesh completed Starter and Engine on both platforms and is the tested
alternative to automatic generation. The Windows automatic-generation trial
produced zero-volume elements with both the rebuilt and the preserved official
Starter. The Windows compatibility helper therefore also selects the explicit
unsupported-keyword diagnostic.

MPI, single precision, other
compiler families and implicit-solver configurations are outside these build
instructions; see [HOWTO.md](../HOWTO.md) for the upstream build options.

## Linux and WSL

Install GCC, G++, GFortran, CMake, Make and Python 3. On Ubuntu:

```bash
sudo apt-get install build-essential gfortran cmake python3
git clone https://github.com/lililii124/openradioss-261001.git
cd openradioss-261001
bash build_linux.sh 8
```

The optional argument is the number of parallel build jobs. In WSL, building in
the Linux filesystem is usually faster than building on a mounted Windows drive.
The executables are `exec/starter_linux64_gf` and `exec/engine_linux64_gf`.

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
the input-reader DLL beside the executables. Use the same oneAPI environment
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

The common dependencies come from
[OpenCourant's recovered v82 tree](https://github.com/OpenCourant/extlib/tree/upstream-v82).
They include LAPACK, METIS, zlib, MD5, Boost, ExprTk, and H3D headers and libraries.
The input readers were recovered separately from public OpenRadioss runtime
packages. Original notices are preserved with the corresponding files.

The helper scripts select the required CMake options. For direct CMake builds:

| Option | Linux reader | Windows reader |
| --- | --- | --- |
| `USE_OPEN_READER` | `0` | `0` |
| `HM_READER_LEGACY_INCLUDE_API` | `ON` | `ON` |
| `HM_READER_LEGACY_PART_API` | `ON` | `OFF` |
| `HM_READER_NO_STRUCTURED_ALE` | `ON` | `ON` |

The three `HM_READER_*` compatibility options default to `ON` in Starter, matching
the recovered Linux reader. The Windows helper explicitly sets the part option
to `OFF` because that reader provides the corresponding interface. The included
OpenReader source does not yet provide every interface required by this Starter
and is not selected by these scripts.

### Using a newer input reader

The v82-hybrid package retains the older input readers. Its version number alone
does not justify disabling reader compatibility options. For a newer reader,
check the required interfaces and run the solver cases before changing those
options. Update the libraries and their manifest entries, then build in a fresh
directory. On Windows, replace the reader DLL in `exec` too, using the import
library supplied with that DLL. The loader only checks local files; changing
the manifest does not download a package.

Check the H3D wrappers against the new SDK headers and rerun the solver cases.
Verify generated node coordinates and element volumes before enabling automatic
structured ALE generation. The Engine startup and SPH comment fixes can remain.

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
