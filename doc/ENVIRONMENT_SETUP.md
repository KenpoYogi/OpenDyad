# Development environment: Windows 11 and WSL openSUSE Tumbleweed

This page lists every component that had to be installed on top of a stock
Windows 11 machine and a stock openSUSE Tumbleweed WSL image to build, test
and package everything in this repository: the Intel MPI + MUMPS Engines, the
GNU + OpenMPI Engine, the single-precision executables, the output converters,
the OpenRadioss GUI and the release bundles. It records the exact packages and
versions of the machine the builds were verified on (October 2026), so the
setup can be reproduced.

[BUILDING.md](BUILDING.md) and [HOWTO_IMPLICIT.md](../HOWTO_IMPLICIT.md)
explain how to run the builds. This page only covers what to install first.

**Contents**

- [Which component is needed for what](#which-component-is-needed-for-what)
- [Windows host](#windows-host)
- [WSL guest: openSUSE Tumbleweed](#wsl-guest-opensuse-tumbleweed)
- [Downloads made during the build](#downloads-made-during-the-build)
- [Installed on the reference machine but not needed](#installed-on-the-reference-machine-but-not-needed)

## Which component is needed for what

| Build output | Platform | Needs |
| --- | --- | --- |
| `starter_win64.exe`, `engine_win64_impi.exe` (implicit, MUMPS) | Windows | Build Tools 2026, Intel oneAPI (ifx, icx, MKL, Intel MPI), Python 3, MUMPS 5.5.1 source |
| `*_sp.exe` single precision, `engine_win64.exe` SMP | Windows | Build Tools 2026, Intel oneAPI, Python 3 |
| `anim_to_vtk_win64.exe`, `th_to_csv_win64.exe` | Windows | Build Tools 2026 (MSVC `cl.exe`) only |
| `OpenRadioss_win64.zip` release bundle | Windows | Python 3, Intel oneAPI (the runtime DLLs are copied from it) |
| `starter_linux64_gf`, `engine_linux64_gf_ompi` (implicit, MUMPS) | WSL | GCC/GFortran, CMake, Make, Python 3, OpenMPI 4 devel |
| `engine_linux64_gf` SMP, `*_gf_sp` single precision | WSL | GCC/GFortran, CMake, Make, Python 3 (OpenMPI 4 for `_ompi_sp`) |
| `starter_linux64_ifx`, `engine_linux64_ifx_impi` | WSL | Intel oneAPI for Linux (ifx, icx, MKL, Intel MPI), MUMPS 5.5.1 source |
| `anim_to_vtk_linux64_gf`, `th_to_csv_linux64_gf` | WSL | GCC and G++ only |
| `OpenRadioss_linux64.zip` release bundle | WSL | Python 3, Intel oneAPI (two runtime libraries are copied from it) |
| OpenRadioss GUI | both | Python 3 with tkinter; on WSL also WSLg for the window |
| Implicit self-test `run_cantilever.*` | both | Python 3 and the MPI runtime of the Engine under test |
| VS Code tasks for the Linux builds | Windows | VS Code with the Remote - WSL extension |

## Windows host

Reference machine: Windows 11 Pro, build 26300. Everything below installs with
defaults unless noted. Install into paths without spaces where you have a
choice; the source tree itself must be in a path without spaces.

### Already in Windows, nothing to install

| Tool | Used by |
| --- | --- |
| `curl.exe`, `tar.exe`, `certutil.exe` | Downloading and checking the MUMPS tarball |
| `cmd.exe` | All `.bat` build scripts |
| `cscript.exe` | `OpenRadioss_gui.vbs`, the GUI launcher in the bundle |
| Windows Subsystem for Linux (feature) | Enabled by `wsl --install`, see below |

### 1. Visual Studio Build Tools 2026

Why: the Intel compilers link with MSVC's `link.exe` and need the MSVC C
runtime and Windows SDK headers and libraries. MSVC `cl.exe` builds the two
output converters. The Build Tools also carry the CMake and Ninja that every
Windows build script uses. The full Visual Studio IDE is not needed.

Download the Build Tools installer from the
[Visual Studio downloads page](https://visualstudio.microsoft.com/downloads/)
(section "Tools for Visual Studio", "Build Tools for Visual Studio 2026").
Select the workload **Desktop development with C++**. The components that the
builds rely on are:

| Component | Installer ID | Reference version |
| --- | --- | --- |
| MSVC v145 x64/x86 build tools | `Microsoft.VisualStudio.Component.VC.Tools.x86.x64` | MSVC 14.51.36231 |
| Windows 11 SDK (10.0.26100) | `Microsoft.VisualStudio.Component.Windows11SDK.26100` | 10.0.26100 |
| C++ CMake tools for Windows | `Microsoft.VisualStudio.Component.VC.CMake.Project` | CMake 4.3.1-msvc1, Ninja 1.13.2 |
| C++ redistributable update | `Microsoft.VisualStudio.Component.VC.Redist.14.Latest` | 14.51.36247 |

Equivalent unattended install, from the folder holding the downloaded
`vs_BuildTools.exe`:

```bat
vs_BuildTools.exe --wait --norestart --passive ^
  --add Microsoft.VisualStudio.Workload.VCTools ^
  --add Microsoft.VisualStudio.Component.VC.Tools.x86.x64 ^
  --add Microsoft.VisualStudio.Component.Windows11SDK.26100 ^
  --add Microsoft.VisualStudio.Component.VC.CMake.Project
```

Notes:

- The install path is `C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools`.
  Intel's `setvars.bat` does not look there on its own. The build scripts set
  `VS2026INSTALLDIR` to that path before calling it; for your own command
  prompts set it yourself (see [HOWTO_IMPLICIT.md](../HOWTO_IMPLICIT.md#troubleshooting)).
- CMake and Ninja are **not** on `PATH` in a plain command prompt. They become
  available once `setvars.bat` (through `vcvarsall.bat`) or
  `VC\Auxiliary\Build\vcvars64.bat` has run. `build_windows_mumps.bat`,
  `build_windows_sp.bat` and `tools\build_output_converters.bat` do this
  themselves. `build_windows_compat.bat` does not: run it from a oneAPI prompt.
- A separate CMake or Ninja installer is not required. If you install one
  anyway, keep CMake at 3.x or 4.x; the scripts use `cmake -S/-B` and Ninja.
- Visual Studio 2022 Build Tools also work; `setvars.bat` finds those on its
  own. The scripts check both `VS2026INSTALLDIR` and `VS2022INSTALLDIR`.

### 2. Intel oneAPI Toolkit 2026.1

Why: the Starter and Engines are built with Intel Fortran (`ifx`) and Intel
C/C++ (`icx`), link against MKL, and the MPI Engines use Intel MPI. The release
bundle copies its OpenMP and MKL runtime DLLs from this installation.

Download the
[Intel oneAPI Toolkit](https://www.intel.com/content/www/us/en/developer/tools/oneapi/toolkits.html)
(in older releases these were the separate Base and HPC toolkits). Use the
default install location `C:\Program Files (x86)\Intel\oneAPI`. Choose a
custom install and make sure these components are selected:

| Component | Folder under `oneAPI\` | Reference version |
| --- | --- | --- |
| Intel Fortran Compiler (`ifx`) | `compiler\2026.1` | 2026.1.1 |
| Intel oneAPI DPC++/C++ Compiler (`icx`) | `compiler\2026.1` | 2026.1.1 |
| Intel oneAPI Math Kernel Library | `mkl\2026.1` | 2026.1 |
| Intel MPI Library | `mpi\2021.18` | 2021.18 |

The installer also adds TBB, oneDPL, UMF and a few utilities. They are not
used, but there is no reason to deselect them. When asked to integrate with
Visual Studio, there is nothing to integrate with Build Tools; skip it.

Notes:

- No "oneAPI command prompt" is needed for `build_windows_mumps.bat` and
  `build_windows_sp.bat`: they call `setvars.bat intel64` when `ifx.exe` is
  not already on `PATH`.
- `mpiexec.exe` lives in `oneAPI\mpi\latest\bin`. The implicit self-test loads
  the MPI environment itself. The GUI needs **Config > MPI Path** set to
  `C:\Program Files (x86)\Intel\oneAPI\mpi\latest`.
- The reference machine also has Intel MPI 2021.14 from an earlier install;
  only one version is needed and `mpi\latest` points at 2021.18.
- The environment variable `NoDefaultCurrentDirectoryInExePath`, if set, breaks
  `setvars.bat`. The scripts clear it; clear it in your own prompts too.

### 3. Python 3.13

Why: the build scripts check the bundled libraries with
`Compiling_tools\script\load_extlib.py`, the bundle is produced by
`make_bundle.py`, the self-test is checked by `check_cantilever.py`, and the
GUI and `inp2rad` are Python/tkinter programs.

Install from [python.org](https://www.python.org/downloads/windows/) or with
winget:

```bat
winget install --id Python.Python.3.13
```

Tick **Add python.exe to PATH** and keep **tcl/tk and IDLE** selected. The
reference machine has Python 3.13.1 with Tk 8.6. No pip packages are needed;
the scripts and the GUI use the standard library only. The Microsoft Store
Python also works for the scripts, but the python.org installer is the one the
GUI documentation assumes.

### 4. Git for Windows (optional)

Only needed to clone the repository and to pull updates; a source ZIP works
without it. Git Bash is convenient for running the `.sh` helpers on Windows
paths, but the Linux builds are done inside WSL.

```bat
winget install --id Git.Git
```

Reference version: 2.53.

### 5. Windows Subsystem for Linux and the openSUSE Tumbleweed distribution

Why: the Linux executables and the Linux release bundle are built in WSL 2.
WSLg, which ships with WSL 2, shows the tkinter GUI from the Linux side.

From an elevated PowerShell:

```powershell
wsl --install -d openSUSE-Tumbleweed
```

Reboot if asked, then set up the Linux user when the distribution starts.
Check the versions with `wsl --version` and `wsl --list --verbose`; the
distribution must be WSL **version 2**.

| Item | Reference version |
| --- | --- |
| WSL | 2.7.14 (Microsoft Store release) |
| Kernel | 6.18.33.2-microsoft-standard-WSL2 |
| WSLg | 1.0.73 |

### 6. Visual Studio Code and extensions (optional)

The `.vscode/tasks.json` tasks run the Windows scripts from a normal window
and the Linux scripts from a **Remote - WSL** window. Install VS Code and the
WSL extension; the Fortran and Python extensions are a convenience.

```bat
winget install --id Microsoft.VisualStudioCode
code --install-extension ms-vscode-remote.remote-wsl
code --install-extension fortran-lang.linter-gfortran
code --install-extension ms-python.python
```

### 7. 7-Zip (optional)

The release bundles are plain zip files, so Explorer can open them. 7-Zip is
faster for the 400 MB Windows bundle.

```bat
winget install --id 7zip.7zip
```

### What a run-only Windows machine needs

A machine that only runs the release bundle, without building, needs:

- Windows 10 or 11 x64.
- The Microsoft Visual C++ 2015-2022 x64 redistributable
  ([vc_redist.x64.exe](https://aka.ms/vs/18/release/vc_redist.x64.exe)).
- The Intel MPI Library runtime, for `engine_win64_impi.exe` and the GUI
  (`mpiexec.exe` and `impi.dll`). The free standalone Intel MPI Library
  installer is enough; a full oneAPI install also works.
- Python 3 with tkinter, for the GUI and `inp2rad`.

The bundle carries the Intel OpenMP and MKL DLLs in
`extlib\intelOneAPI_runtime\win64`, so the compilers and MKL are not needed.

### Verify the Windows installation

```bat
set "VS2026INSTALLDIR=C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools"
set NoDefaultCurrentDirectoryInExePath=
call "C:\Program Files (x86)\Intel\oneAPI\setvars.bat" intel64
where ifx icx link cl cmake ninja mpiexec python
ifx /QV
cmake --version
python -c "import tkinter; print('tkinter', tkinter.TkVersion)"
```

All eight executables should be found. Then follow the
[MUMPS download](../HOWTO_IMPLICIT.md#get-the-mumps-source) and build steps.

## WSL guest: openSUSE Tumbleweed

Reference instance: openSUSE Tumbleweed snapshot 20261003, glibc 2.44, kernel
6.18 (WSL 2). The WSL image is the `minimal_base` pattern: it has `bash`,
`curl`, `tar`, `gzip`, `zip` and `wget`, and little else. Everything in this
section was added with `zypper`. The commands are idempotent, so run them all
even if some packages are already present.

Tumbleweed is a rolling release. Update before installing anything, and expect
newer versions than the ones listed:

```bash
sudo zypper refresh && sudo zypper dup
```

### 1. GNU toolchain, CMake, Ninja, Make, Python 3

Why: `build_linux.sh`, `build_linux_gf_mumps.sh`, `build_linux_gf_sp.sh` and
`tools/build_output_converters.sh` compile with GCC, G++ and GFortran through
CMake. `make` builds LAPACK and MUMPS inside the GNU MUMPS script. `which` is
used by OpenMPI's wrappers. Python 3 runs the same helper scripts as on Windows.

```bash
sudo zypper install gcc gcc-c++ gcc-fortran cmake ninja make which python3 git curl tar
```

What that installs on the reference instance:

| Package requested | Resolved to | Version |
| --- | --- | --- |
| `gcc`, `gcc-c++`, `gcc-fortran` | `gcc16`, `gcc16-c++`, `gcc16-fortran` | 16.2.1 |
| `cmake` | `cmake`, `cmake-full` | 4.4.3 |
| `ninja` | `ninja` | 1.13.2 |
| `make` | `make` | 4.4.1 |
| `which` | `which` | 2.25 |
| `python3` | `python313`, `python313-pip` | 3.13.15 |
| `git` | `git`, `git-core` | 2.55 |
| pulled in automatically | `binutils`, `glibc-devel`, `libstdc++-devel`, `libgfortran5`, `libgomp1` | |

Notes:

- GCC 14 and newer reject old Fortran argument mismatches and old C idioms.
  The GNU MUMPS script passes the needed `-fallow-argument-mismatch` and
  `-Wno-error=...` flags for LAPACK, ScaLAPACK and MUMPS, so no older GCC is
  required.
- CMake 4 refuses projects that ask for CMake older than 3.5, which ScaLAPACK
  2.2.0 does. The script sets `CMAKE_POLICY_VERSION_MINIMUM=3.5`. No downgrade
  is needed.
- No pip packages are needed.

### 2. OpenMPI 4

Why: `engine_linux64_gf_ompi` and `engine_linux64_gf_ompi_sp` are built with
OpenMPI's `mpicc`/`mpif90` wrappers, as are ScaLAPACK and MUMPS in the GNU
build. Install the **openmpi4** flavour, not OpenMPI 5: the scripts, the GUI
and the bundle README expect `libmpi.so.40`.

```bash
sudo zypper install openmpi4-devel
```

This installs `openmpi4` 4.1.8, `openmpi4-libs`, `openmpi4-config`,
`openmpi4-devel`, `mpi-selector`, and the fabric libraries they depend on
(`rdma-core`, `libfabric1`, `libpsm2`, UCX, `libevent`). On WSL those fabric
libraries are unused but harmless.

openSUSE installs OpenMPI under `/usr/lib64/mpi/gcc/openmpi4`, and does
**not** put its `bin` on `PATH`. The build and run scripts handle this through
`OPENMPI_ROOT`, which defaults to that prefix. For interactive use:

```bash
export PATH=/usr/lib64/mpi/gcc/openmpi4/bin:$PATH
export LD_LIBRARY_PATH=/usr/lib64/mpi/gcc/openmpi4/lib64:${LD_LIBRARY_PATH:-}
```

The GUI needs **Config > MPI Path** set to `/usr/lib64/mpi/gcc/openmpi4`.

### 3. Intel oneAPI for Linux (optional)

Only needed for `starter_linux64_ifx` and `engine_linux64_ifx_impi`, built by
`build_linux_mumps.sh`, and for `make_bundle.py --os linux64`, which copies
`libirng.so` and `libintlc.so.5` from the compiler into the bundle. Skip this
section if you only want the GNU executables. It takes about 5.7 GB under
`/opt/intel/oneapi`.

Add Intel's repository, import its signing key and install the same versions
as on Windows:

```bash
sudo zypper addrepo https://yum.repos.intel.com/oneapi oneAPI
sudo rpm --import https://yum.repos.intel.com/intel-gpg-keys/GPG-PUB-KEY-INTEL-SW-PRODUCTS.PUB
sudo zypper install intel-oneapi-compiler-fortran-2026.1 \
                    intel-oneapi-compiler-dpcpp-cpp-2026.1 \
                    intel-oneapi-mkl-devel-2026.1 \
                    intel-oneapi-mpi-devel-2021.18
```

| Package requested | Provides | Reference version |
| --- | --- | --- |
| `intel-oneapi-compiler-fortran-2026.1` | `ifx`, Fortran runtime | 2026.1.1 |
| `intel-oneapi-compiler-dpcpp-cpp-2026.1` | `icx`, `icpx`, OpenMP runtime | 2026.1.1 |
| `intel-oneapi-mkl-devel-2026.1` | MKL headers and libraries, including the cluster (ScaLAPACK/BLACS) parts | 2026.1.0 |
| `intel-oneapi-mpi-devel-2021.18` | Intel MPI compiler wrappers, `mpiexec`, `libmpi.so.12` | 2021.18.1 |

The dependency solver adds the MKL SYCL and classic sub-packages, TBB, UMF,
the oneAPI common variables and the DPC++ debugger. Pinning the versions in the
package names keeps `zypper dup` from pulling a newer compiler unasked.

`build_linux_mumps.sh` and `run_cantilever.sh` source
`/opt/intel/oneapi/setvars.sh` themselves when `ifx` or Intel's `mpiexec` is
missing from `PATH`. For interactive use run `source /opt/intel/oneapi/setvars.sh`.

### 4. tkinter for the GUI

Why: `openradioss_gui` and `inp2rad` are tkinter programs. The openSUSE Python
package does not include tkinter.

```bash
sudo zypper install python313-tk
```

This brings `tk` and `tcl` 8.6.18. The window is displayed through WSLg; no X
server on Windows is required. The reference instance also has the
`wsl_gui` pattern (`sudo zypper install -t pattern wsl_gui`), which adds fonts
and GUI libraries; it is optional but gives nicer rendering.

### 5. unzip (optional)

Not installed on the reference instance, but needed to unpack a Linux release
bundle in a way that keeps the executable permissions:

```bash
sudo zypper install unzip
```

### Not needed in WSL

- Distribution packages for LAPACK, ScaLAPACK or MUMPS. The GNU MUMPS script
  downloads and builds its own copies under `engine/extlib/`, and the Intel
  build uses MKL plus the MUMPS source you extract.
- Apptainer, Git LFS, Docker: the `Apptainer/` definitions are upstream
  artefacts for Rocky Linux and are not used here.
- An X server on Windows (VcXsrv, Xming). WSLg does this.

### Where to put the source in WSL

Clone into the Linux filesystem, not under `/mnt/c`; builds there are several
times faster. On the reference machine the WSL clone lives in the home
directory and uses the Windows checkout as its Git remote, so commits made on
either side can be fetched by the other:

```bash
git clone /mnt/c/Users/<you>/source/repos/OpenDyad ~/OpenDyad
cd ~/OpenDyad
```

The dependency builds and the MUMPS source live inside this clone
(`engine/extlib/`) and are ignored by Git.

### Verify the WSL installation

```bash
gcc --version | head -1; gfortran --version | head -1
cmake --version | head -1; ninja --version; make --version | head -1
python3 -c "import tkinter, sys; print(sys.version.split()[0], 'tkinter', tkinter.TkVersion)"
/usr/lib64/mpi/gcc/openmpi4/bin/mpirun --version | head -1
/usr/lib64/mpi/gcc/openmpi4/bin/mpif90 --version | head -1
ldd --version | head -1
# Intel only:
source /opt/intel/oneapi/setvars.sh >/dev/null && ifx --version | head -1 && mpiexec --version | head -1
```

Expected on the reference instance: GCC and GFortran 16.2.1, CMake 4.4.3,
Ninja 1.13.2, Make 4.4.1, Python 3.13.15 with Tk 8.6, Open MPI 4.1.8 with
`mpif90` wrapping GFortran, glibc 2.44, and for Intel ifx 2026.1.1 and Intel
MPI 2021.18.

## Downloads made during the build

The build scripts fetch these themselves, or ask you to, so the machine needs
internet access the first time. Checksums are verified where a script downloads.

| What | Where it lands | Who downloads it |
| --- | --- | --- |
| MUMPS 5.5.1 source (`ftp.mcs.anl.gov`, PETSc mirror) | `engine/extlib/MUMPS_5.5.1` | You, with `curl` and `tar`, see [HOWTO_IMPLICIT.md](../HOWTO_IMPLICIT.md#get-the-mumps-source). Needed by the Intel builds on both platforms. |
| LAPACK 3.10.1 (GitHub) | `engine/extlib/lapack-3.10.1` | `build_linux_gf_mumps.sh` |
| ScaLAPACK 2.2.0 (GitHub) | `engine/extlib/scalapack-2.2.0` | `build_linux_gf_mumps.sh` |
| MUMPS 5.5.1 for the GNU build | `engine/extlib/MUMPS_5.5.1_gf_ompi` | `build_linux_gf_mumps.sh` (separate copy, built with OpenMPI wrappers) |

The solver's other dependencies (LAPACK for the Starter, METIS, zlib, Boost,
the H3D libraries and the input readers) are checked in under `extlib/` and
need no download.

## Installed on the reference machine but not needed

For completeness, these are present on the reference machine but play no part
in building or running OpenRadioss. Do not install them for this project.

| Where | Component | Note |
| --- | --- | --- |
| Windows | Intel MPI Library 2021.14 (second copy) | Left from an earlier install; 2021.18 is used |
| Windows | Build Tools components: AddressSanitizer, vcpkg, Windows 10 SDK, MSBuild Tools workload | Default extras of the C++ workload |
| Windows | pip packages numpy, pillow, PyMuPDF | Unrelated to this repository |
| WSL | `zfs`, `zfs-kmp-default`, the `filesystems` OBS repository, `btrfsprogs`, `ntfs-3g` | Filesystem tooling |
| WSL | `nmap`, `nano`, `gitk`, `git-gui` | Utilities |
| WSL | `gcc13` | Pulled in as a dependency; GCC 16 does the builds |
