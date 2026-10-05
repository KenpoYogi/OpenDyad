# HOWTO: implicit analysis with MUMPS

This fork builds OpenRadioss with the [MUMPS](https://mumps-solver.org) 5.5.1
direct solver, so implicit analyses (`/IMPLICIT`, `/IMPL/...`) run on Windows
and Linux. Without MUMPS the implicit solver is compiled out of the Engine,
which then stops with `Fatal error: MUMPS required`.

This guide covers prerequisites, building, verifying a build, running implicit
models, the GUI, release bundles and known problems.

**Contents**

- [What is supported](#what-is-supported)
- [Prerequisites](#prerequisites)
- [Get the MUMPS source](#get-the-mumps-source)
- [Build](#build)
- [Verify the build](#verify-the-build)
- [Run an implicit model](#run-an-implicit-model)
- [OpenRadioss GUI](#openradioss-gui)
- [Release bundles](#release-bundles)
- [Changes from the upstream snapshot](#changes-from-the-upstream-snapshot)
- [Troubleshooting](#troubleshooting)
- [Licensing](#licensing)

## What is supported

| Platform | Engine | Toolchain | Build script |
| --- | --- | --- | --- |
| Windows x86-64 | `engine_win64_impi.exe` | Intel oneAPI (ifx, icx, MKL) + Intel MPI | [`build_windows_mumps.bat`](build_windows_mumps.bat) |
| Linux x86-64 | `engine_linux64_ifx_impi` | Intel oneAPI (ifx, icx, MKL) + Intel MPI | [`build_linux_mumps.sh`](build_linux_mumps.sh) |
| Linux x86-64 | `engine_linux64_gf_ompi` | GCC/GFortran + OpenMPI 4 | [`build_linux_gf_mumps.sh`](build_linux_gf_mumps.sh) |

- **MPI engines only.** MUMPS needs MPI, so only the MPI Engines contain the implicit solver. They also run on a single process (`mpiexec -n 1`). The SMP Engines (`engine_win64.exe`, `engine_linux64_gf`) run explicit models only.
- **Double precision only.**
- **Verified:** linear static analysis, against a beam-theory reference, on all three Engines with 1 and 2 MPI processes. See [Verify the build](#verify-the-build).
- **Not verified here:** nonlinear static (`/IMPL/NONLIN`) and implicit dynamic (`/IMPL/DYNA`). They are compiled in.
- **Not available:** `/IMPL/BUCKL` and the Engine `/EIG` option are compiled out upstream, by a macro that no build defines.

Tested toolchains:

| | Version |
| --- | --- |
| Intel oneAPI | 2026.1 (ifx/icx 2026.1.1, MKL 2026.1, Intel MPI 2021.18) |
| Windows | Visual Studio Build Tools 2026 (MSVC 14.51), Windows SDK 10.0.26100 |
| Linux | openSUSE Tumbleweed (WSL2), GCC 16.2, CMake 4.4, OpenMPI 4.1.8 |

## Prerequisites

Clone or extract the source into a path **without spaces**; some upstream
compiler flags do not support spaces.

### Windows

- **Intel oneAPI** with the Fortran compiler, the DPC++/C++ compiler, MKL and Intel MPI. The Base and HPC toolkits contain all of them.
- **Visual Studio Build Tools**, with the MSVC x64 tools and a Windows SDK ("Desktop development with C++"). The Build Tools include CMake and Ninja.
- **Python 3**. Also needed for the self-test and the GUI.

You do not need a oneAPI command prompt: the build script loads the oneAPI
environment itself. It also handles Build Tools 2026, which oneAPI's
`setvars.bat` does not find on its own.

### Linux (openSUSE Tumbleweed)

```bash
sudo zypper refresh && sudo zypper dup
sudo zypper install gcc gcc-c++ gcc-fortran cmake ninja make which python3
```

For the **Intel** Engine, add Intel's repository and install the same versions
as on Windows:

```bash
sudo zypper addrepo https://yum.repos.intel.com/oneapi oneAPI
sudo rpm --import https://yum.repos.intel.com/intel-gpg-keys/GPG-PUB-KEY-INTEL-SW-PRODUCTS.PUB
sudo zypper install intel-oneapi-compiler-fortran-2026.1 intel-oneapi-compiler-dpcpp-cpp-2026.1 \
                    intel-oneapi-mkl-devel-2026.1 intel-oneapi-mpi-devel-2021.18
```

For the **GNU** Engine, and for the GUI:

```bash
sudo zypper install openmpi4-devel     # GNU + OpenMPI Engine
sudo zypper install python313-tk       # OpenRadioss GUI
```

Other distributions need the equivalent packages. The GNU script takes OpenMPI
from `OPENMPI_ROOT`, which defaults to `/usr/lib64/mpi/gcc/openmpi4`. Point it
at an OpenMPI 4 prefix that contains `bin/mpif90`, `include/` and `lib64/` or
`lib/`.

In WSL, build in the Linux filesystem (for example `~/openradioss-261001`)
rather than under `/mnt/c`; it is much faster.

## Get the MUMPS source

The Intel builds compile MUMPS directly into the Engine from
`engine/extlib/MUMPS_5.5.1`. That folder is not in Git, so download it once.

Windows (`curl` and `tar` are built into Windows 10 and 11):

```bat
cd engine\extlib
curl -L -o MUMPS_5.5.1.tar.gz https://ftp.mcs.anl.gov/pub/petsc/externalpackages/MUMPS_5.5.1.tar.gz
certutil -hashfile MUMPS_5.5.1.tar.gz SHA256
tar -xzf MUMPS_5.5.1.tar.gz
del MUMPS_5.5.1.tar.gz
```

Linux:

```bash
cd engine/extlib
curl -fsSL -o MUMPS_5.5.1.tar.gz https://ftp.mcs.anl.gov/pub/petsc/externalpackages/MUMPS_5.5.1.tar.gz
echo "1abff294fa47ee4cfd50dfd5c595942b72ebfcedce08142a75a99ab35014fa15  MUMPS_5.5.1.tar.gz" | sha256sum -c -
tar -xzf MUMPS_5.5.1.tar.gz && rm MUMPS_5.5.1.tar.gz
```

The SHA-256 checksum must be
`1abff294fa47ee4cfd50dfd5c595942b72ebfcedce08142a75a99ab35014fa15`.

Keep MUMPS at 5.5.1. The Engine's CMake filter that picks the double-precision
sources was written for that layout.

The GNU build downloads its own MUMPS, LAPACK and ScaLAPACK (see below).

## Build

All scripts take an optional number of parallel jobs and write to `exec/`.

### Windows

```bat
build_windows_mumps.bat 16
```

This builds `starter_win64.exe` and `engine_win64_impi.exe`, in about
8 minutes with 28 parallel jobs.

Optional extras:

```bat
rem Output converters anim_to_vtk_win64.exe and th_to_csv_win64.exe (finds MSVC itself)
tools\build_output_converters.bat

rem SMP Engine engine_win64.exe (explicit only). This script needs oneAPI loaded first.
set "VS2026INSTALLDIR=C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools"
call "C:\Program Files (x86)\Intel\oneAPI\setvars.bat"
build_windows_compat.bat 16
```

### Linux, Intel

```bash
bash build_linux_mumps.sh 8
```

Builds `starter_linux64_ifx` and `engine_linux64_ifx_impi`. The script sources
`/opt/intel/oneapi/setvars.sh` when `ifx` is not on `PATH`.

### Linux, GNU + OpenMPI

```bash
bash build_linux_gf_mumps.sh 8
```

The first run builds LAPACK 3.10.1, ScaLAPACK 2.2.0 and MUMPS 5.5.1 with
OpenMPI's compiler wrappers, in `engine/extlib/`. The downloads are checksum
verified. Later runs reuse them. The script then builds `starter_linux64_gf`
and `engine_linux64_gf_ompi`.

Optional extras:

```bash
bash build_linux.sh 8                   # SMP Engine engine_linux64_gf (explicit only)
bash tools/build_output_converters.sh   # anim_to_vtk_linux64_gf and th_to_csv_linux64_gf
```

### VS Code

[`.vscode/tasks.json`](.vscode/tasks.json) wraps these scripts:

| Task | What it does |
| --- | --- |
| Build Starter + Engine (Intel MPI, MUMPS) | The default build task (Ctrl+Shift+B). Runs the Windows script, or the Linux Intel script in a Remote-WSL window. |
| Linux: build Starter + Engine (GNU + OpenMPI, MUMPS) | Runs `build_linux_gf_mumps.sh`. |
| Build output converters (anim_to_vtk, th_to_csv) | Builds both converters. |
| Test implicit cantilever (MUMPS) | Runs the self-test below. |
| Package release bundle | Writes the release zip; see [Release bundles](#release-bundles). |

## Verify the build

[`qa-tests/implicit/cantilever`](qa-tests/implicit/cantilever/README.md) is a
linear static shell cantilever with a closed-form answer: tip deflection
P·L³/(3EI) = **2.000 mm**. The run script copies the decks to a temporary
folder, runs Starter and Engine, and checks the listing and the displacements.

```bat
qa-tests\implicit\cantilever\run_cantilever.bat engine_win64_impi 2
```

```bash
bash qa-tests/implicit/cantilever/run_cantilever.sh engine_linux64_gf_ompi 2
bash qa-tests/implicit/cantilever/run_cantilever.sh engine_linux64_ifx_impi 2
```

The arguments are the Engine and the number of MPI processes. A good build prints:

```
  tip mean DZ = 1.999808 mm, expected 2.000000 mm (-0.010 %)
=== PASS  (listing ok, displacements ok, tolerance 0.5 % of tip)
```

All three Engines give 1.999808 mm on 1 and 2 processes. All MPI and SMP
Engines also complete the explicit smoke test (`qa-tests/miniqa/SMOKE_TEST`)
in the same 16,796 cycles.

## Run an implicit model

### Input decks

The **Starter** deck needs the `/IMPLICIT` card. It has no data lines.

The **Engine** deck selects the analysis. This minimal linear static setup is
taken from the self-test:

```
/RUN/model/1
                 1.0
/IMPL/LINEAR
/IMPL/SOLVER/2
#    IPREC     L_LIM      ITOL               L_TOL
         0         0         0                 0.0
/IMPL/PRINT/LINEAR/-1
/IMPL/MUMPS/MSGLV/2
```

| Card | Effect |
| --- | --- |
| `/IMPL/LINEAR` | One linear step from 0 to the `/RUN` end time. |
| `/IMPL/SOLVER/2` | Direct solver, MUMPS. Its data line must contain all four values. |
| `/IMPL/PRINT/LINEAR/-1` | Prints the solver residual, `DIRECT SOLVER TERMINATED WITH RELATIVE ‖R‖=`. |
| `/IMPL/MUMPS/MSGLV/2` | Copies MUMPS's own messages into the Engine listing. Optional. |

- Static is the default when there is no `/IMPL/DYNA`. `/IMPL/STATIC` is not a keyword, and an unknown `/IMPL` key stops the Engine.
- Use shell formulations with implicit stiffness: Ishell 24 (QEPH) or 12 (QBAT). Ishell 1 to 4 print "not available for stiffness matrix".
- The full option list is in the [Radioss reference guide](https://2022.help.altair.com/2022/simulation/pdfs/radopen/AltairRadioss_2022_ReferenceGuide.pdf) under `/IMPL`.

### Command line

Starter `-np N` splits the model into N domains. Run the Engine with the same
number of MPI processes.

Windows, from a source build:

```bat
call "C:\Program Files (x86)\Intel\oneAPI\setvars.bat"
set "OPENRADIOSS_PATH=C:\path\to\openradioss-261001"
set "RAD_CFG_PATH=%OPENRADIOSS_PATH%\hm_cfg_files"
set "RAD_H3D_PATH=%OPENRADIOSS_PATH%\extlib\h3d\lib\win64"
set "KMP_STACKSIZE=400m"
set "PATH=%OPENRADIOSS_PATH%\extlib\hm_reader\win64;%PATH%"

"%OPENRADIOSS_PATH%\exec\starter_win64.exe" -i model_0000.rad -np 2
mpiexec -n 2 "%OPENRADIOSS_PATH%\exec\engine_win64_impi.exe" -i model_0001.rad
```

If `setvars.bat` reports that Visual Studio was not found, ignore it. Running
needs only the compiler and MPI parts of the environment.

Linux, GNU + OpenMPI:

```bash
export OPENRADIOSS_PATH=~/openradioss-261001
export RAD_CFG_PATH=$OPENRADIOSS_PATH/hm_cfg_files
export RAD_H3D_PATH=$OPENRADIOSS_PATH/extlib/h3d/lib/linux64
export OMP_STACKSIZE=400m
export PATH=/usr/lib64/mpi/gcc/openmpi4/bin:$PATH
export LD_LIBRARY_PATH=$OPENRADIOSS_PATH/extlib/hm_reader/linux64:/usr/lib64/mpi/gcc/openmpi4/lib64:${LD_LIBRARY_PATH:-}

$OPENRADIOSS_PATH/exec/starter_linux64_gf -i model_0000.rad -np 2
mpiexec -n 2 $OPENRADIOSS_PATH/exec/engine_linux64_gf_ompi -i model_0001.rad
```

For the Intel Engine on Linux, run `source /opt/intel/oneapi/setvars.sh`
instead of the two OpenMPI lines, and run `engine_linux64_ifx_impi`. Restart
files from `starter_linux64_gf` also run on `engine_linux64_ifx_impi`; the
release bundle ships only that Starter.

### Reading the Engine listing

A good implicit run's `model_0001.out` shows:
- `IMPLICIT TYPE ... STATIC LINEAR` and `LINEAR SOLVER ... DIRECT(MUMPS)`
- MUMPS's analysis, factorization and solve steps (with `/IMPL/MUMPS/MSGLV/2`)
- the solver residual
- `NORMAL TERMINATION`

One message is normal: `Warning: MUMPS workspace too small. Retry`, or
`ERROR RETURN FROM DMUMPS INFO(1)= -19` on the console. On the first
factorization, the Engine's memory estimate for MUMPS is often too small. It
retries with more memory by design, and the second factorization succeeds.

## OpenRadioss GUI

The GUI ([`tools/openradioss_gui`](tools/openradioss_gui/README.md)) is a
Python/tkinter launcher. It needs Python 3 with tkinter: the python.org
installer on Windows, `python313-tk` on openSUSE, `python3-tk` on Debian and
Ubuntu.

In this fork the GUI runs **every job on the MPI Engine**, explicit or
implicit, through `mpiexec`/`mpirun`, also with one process:
`engine_win64_impi` on Windows and `engine_linux64_gf_ompi` on Linux.

Set **Config > MPI Path** once:

| Platform | MPI Path |
| --- | --- |
| Windows (Intel MPI) | `C:\Program Files (x86)\Intel\oneAPI\mpi\latest` |
| openSUSE (OpenMPI 4) | `/usr/lib64/mpi/gcc/openmpi4` |
| Other OpenMPI | its install prefix, for example `/opt/openmpi` |

Start it from a release bundle with `openradioss_gui\OpenRadioss_gui.vbs`
(Windows) or `bash openradioss_gui/OpenRadioss_gui.bash` (Linux). Batch mode
runs the same steps without a window:

```bash
python3 openradioss_gui/OpenRadioss_gui.py -i model_0000.rad -np 2 \
        -th_to_csv -anim_to_vtk -mpi_path /usr/lib64/mpi/gcc/openmpi4
```

The GUI expects the release-bundle layout: `exec/` and `hm_cfg_files/` one
level above its own folder. It converts Abaqus `.inp` input through
`inp2rad.py`, which the bundles place beside it.

## Release bundles

[`Compiling_tools/script/make_bundle.py`](Compiling_tools/script/make_bundle.py)
packages the built executables into `bundles/OpenRadioss_<os>.zip`. The layout
follows the upstream release workflow:

```bash
python Compiling_tools/script/make_bundle.py --os win64      # on Windows
python3 Compiling_tools/script/make_bundle.py --os linux64   # on Linux
```

```
OpenRadioss/
  exec/                               Starter, Engines, anim_to_vtk, th_to_csv
  extlib/h3d/, extlib/hm_reader/      reader and H3D libraries
  extlib/intelOneAPI_runtime/<os>/    Intel runtime libraries the executables load
  hm_cfg_files/                       input format definitions
  openradioss_gui/                    GUI, with inp2rad.py
  qa-tests/implicit/cantilever/       the implicit self-test
  licenses/, LICENSE.md, COPYRIGHT.md, README.txt
```

Build everything you want to ship first: the MUMPS builds, the optional SMP
Engines and the converters. Executables that are missing are left out, with a
warning. `README.txt` inside each zip records the commit and the toolchain
versions.

Requirements on the target machine:

| Bundle | Needs |
| --- | --- |
| Windows | Windows 10/11 x64. The Visual C++ 2015-2022 redistributable. The Intel MPI runtime for `engine_win64_impi.exe`. |
| Linux, GNU executables | glibc 2.44 or newer (because they were built on Tumbleweed) and OpenMPI 4.x. |
| Linux, `engine_linux64_ifx_impi` | glibc 2.38 or newer, for example Ubuntu 24.04, and the Intel MPI runtime. |

To check a bundle on the target machine, unzip it and run its self-test,
`qa-tests/implicit/cantilever/run_cantilever.*`. On Linux, use `unzip` so the
executable permissions are kept.

## Changes from the upstream snapshot

| Change | Files |
| --- | --- |
| Fix an Engine crash at the start of every implicit run with ifx 2026.1 optimized builds. The compiler dropped a loop's zero-trip test in `DIM_KINMAX` when `NKINE=0`. | [`engine/source/implicit/ind_glob_k.F`](engine/source/implicit/ind_glob_k.F) |
| Pass the MUMPS flags to the per-file flag sets on Linux. Files such as `resol.F` were built without `-DMUMPS5`. | [`engine/CMake_Compilers/cmake_linux64_ifx.txt`](engine/CMake_Compilers/cmake_linux64_ifx.txt), [`cmake_linux64_ifort.txt`](engine/CMake_Compilers/cmake_linux64_ifort.txt) |
| Build scripts for the three MPI + MUMPS Engines. | `build_windows_mumps.bat`, `build_linux_mumps.sh`, `build_linux_gf_mumps.sh` |
| Implicit self-test. | [`qa-tests/implicit/cantilever`](qa-tests/implicit/cantilever/README.md) |
| Release bundle packaging. | [`Compiling_tools/script/make_bundle.py`](Compiling_tools/script/make_bundle.py) |
| Output converters, GUI and inp2rad sources, from [OpenCourant/Tools](https://github.com/OpenCourant/Tools) (the deleted OpenRadioss/Tools repository's history). The converters are built with optimization. The GUI always uses the MPI Engine and finds OpenMPI in `lib64`. | [`tools/`](tools) |
| VS Code tasks. | [`.vscode/tasks.json`](.vscode/tasks.json) |

## Troubleshooting

| Symptom | Cause and fix |
| --- | --- |
| `Fatal error: MUMPS required` on the console | The Engine has no MUMPS. Use an MPI Engine (`*_impi`, `*_ompi`), not an SMP Engine. For a source build, check that the configure step printed `MUMPS is enabled` (Intel) and that `engine/extlib/MUMPS_5.5.1` exists. |
| Engine crashes (`forrtl: severe (157)` access violation) right after `SOLUTION PHASE` on implicit models only | The Engine was built without the `DIM_KINMAX` fix in `ind_glob_k.F`. Rebuild from this branch. |
| `setvars.bat`: `Visual Studio was not found` while building | `setvars.bat` does not find Build Tools 2026. `build_windows_mumps.bat` sets `VS2026INSTALLDIR` for you. For your own scripts, set `VS2026INSTALLDIR=C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools` first. |
| `'vars.bat' is not recognized` from `setvars.bat` | The environment variable `NoDefaultCurrentDirectoryInExePath` is set. Clear it before calling `setvars.bat`; the scripts in this repo do. |
| ScaLAPACK: `Compatibility with CMake < 3.5 has been removed` | CMake 4 with ScaLAPACK 2.2.0. `build_linux_gf_mumps.sh` sets `CMAKE_POLICY_VERSION_MINIMUM=3.5` in the environment, so it also reaches ScaLAPACK's nested configure. |
| `libintlc.so.5` or `libirng.so: cannot open shared object file` | The Intel Engine on Linux needs `extlib/intelOneAPI_runtime/linux64` on `LD_LIBRARY_PATH`, or a oneAPI installation. |
| `GLIBC_2.44 not found` | The Linux GNU executables were built on Tumbleweed. Use a newer distribution, or `engine_linux64_ifx_impi` (needs glibc 2.38), or build on the target system. |
| `libmpi.so.40: cannot open shared object file` | OpenMPI 4 is not on `LD_LIBRARY_PATH`. Add its `lib64` (or `lib`) folder, or set the GUI's MPI Path. |
| GUI: `No module named 'tkinter'` | Install tkinter: `python313-tk` on openSUSE, `python3-tk` on Debian and Ubuntu. |
| Starter: `Include file ... not found` | Copy the deck's `#include` files with it. Some QA decks have extra files, such as `TWISBEAMD0B`. |

## Licensing

OpenRadioss is under the [GNU AGPL v3](LICENSE.md). The Engines built here
contain modified OpenRadioss source, so if you distribute them, the AGPL
requires you to offer the corresponding source.

MUMPS is under the CeCILL-C license. ScaLAPACK and LAPACK are under BSD-style
licenses. The converters, GUI and inp2rad are under the MIT license. The
bundles also include the redistributable Intel runtime libraries, under
Intel's license. Each bundle's `licenses/` folder contains all of these license
texts.
