#!/usr/bin/env bash
# Default full build. One command builds everything the Linux release bundle ships:
#   1. starter_linux64_gf, engine_linux64_gf_ompi
#        double precision, GNU + OpenMPI 4 + MUMPS: explicit and implicit  (build_linux_gf_mumps.sh;
#        builds LAPACK, ScaLAPACK and MUMPS into engine/extlib on the first run)
#   2. starter_linux64_gf_sp, engine_linux64_gf_sp, engine_linux64_gf_ompi_sp
#        single precision, explicit only                                  (build_linux_gf_sp.sh)
#   3. anim_to_vtk_linux64_gf, th_to_csv_linux64_gf
#        output converters                                                (tools/build_output_converters.sh)
#   4. openradioss_gui/ at the repo root
#        the OpenRadioss GUI with inp2rad.py, staged to run from here      (tools/stage_gui.py)
#   5. starter_linux64_ifx, engine_linux64_ifx_impi
#        double precision, Intel oneAPI + Intel MPI + MUMPS                (build_linux_mumps.sh)
#        Only when Intel oneAPI is installed (ifx on PATH, or ONEAPI_ROOT / /opt/intel/oneapi).
#        Downloads MUMPS 5.5.1 into engine/extlib when it is missing (SHA-256 checked).
# OpenMPI 4 comes from OPENMPI_ROOT, default /usr/lib64/mpi/gcc/openmpi4 (openSUSE openmpi4-devel).
#
# Usage: bash build_linux_all.sh [jobs] [-clean] [-smp] [-no-intel] [-bundle]
#   jobs       parallel build jobs (default: all processors)
#   -clean     delete the Starter and Engine build directories (cbuild_linux64*) first.
#              Needed after moving or renaming the source tree: CMake refuses a stale cache.
#              The LAPACK, ScaLAPACK and MUMPS builds in engine/extlib are kept.
#   -smp       also build the OpenMP-only double-precision Engine engine_linux64_gf
#              (explicit only; the MPI Engine already runs explicit models on one rank)
#   -no-intel  skip the Intel Engine even when oneAPI is installed
#   -bundle    package exec/ into bundles/OpenRadioss_linux64.zip afterwards
set -euo pipefail

root=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
jobs=
clean=0
smp=0
intel=auto
bundle=0

usage() {
    printf 'Usage: bash build_linux_all.sh [jobs] [-clean] [-smp] [-no-intel] [-bundle]\n'
    printf '  jobs       number of parallel build jobs (default: all processors)\n'
    printf '  -clean     delete the Starter and Engine build directories first (after moving the source tree)\n'
    printf '  -smp       also build the OpenMP-only double-precision Engine engine_linux64_gf\n'
    printf '  -no-intel  skip the Intel oneAPI Engine even when oneAPI is installed\n'
    printf '  -bundle    package exec/ into bundles/OpenRadioss_linux64.zip afterwards\n'
}

for arg in "$@"; do
    case "$arg" in
        -clean|--clean) clean=1 ;;
        -smp|--smp) smp=1 ;;
        -no-intel|--no-intel) intel=skip ;;
        -bundle|--bundle) bundle=1 ;;
        -h|--help) usage; exit 0 ;;
        *)
            if [[ "$arg" =~ ^[1-9][0-9]*$ ]]; then
                jobs=$arg
            else
                usage >&2
                exit 1
            fi ;;
    esac
done
: "${jobs:=$(getconf _NPROCESSORS_ONLN)}"

# Intel oneAPI: build the Intel Engine when the compiler is reachable.
oneapi_root=${ONEAPI_ROOT:-/opt/intel/oneapi}
if [ "$intel" = auto ]; then
    if command -v ifx >/dev/null 2>&1 || [ -f "$oneapi_root/setvars.sh" ]; then
        intel=yes
    else
        intel=no
    fi
fi

printf '\nOpenDyad full build: double precision GNU + OpenMPI + MUMPS, single precision, output converters, GUI\n'
[ "$intel" = yes ] && printf '  plus the Intel oneAPI + Intel MPI + MUMPS Engine\n'
[ "$smp" = 1 ] && printf '  plus the OpenMP-only double-precision Engine\n'
[ "$bundle" = 1 ] && printf '  plus the release bundle\n'
[ "$clean" = 1 ] && printf '  from clean build directories\n'
printf '  %s parallel jobs\n\n' "$jobs"

for tool in gfortran gcc g++ cmake make python3 curl tar; do
    if ! command -v "$tool" >/dev/null 2>&1; then
        printf '%s is not installed. See doc/ENVIRONMENT_SETUP.md.\n' "$tool" >&2
        exit 1
    fi
done

if [ "$clean" = 1 ]; then
    for d in starter/cbuild_linux64_gf starter/cbuild_linux64_gf_sp starter/cbuild_linux64_ifx \
             engine/cbuild_linux64_gf engine/cbuild_linux64_gf_ompi engine/cbuild_linux64_gf_sp \
             engine/cbuild_linux64_gf_ompi_sp engine/cbuild_linux64_ifx_impi; do
        if [ -d "$root/$d" ]; then
            printf 'Removing %s\n' "$d"
            rm -rf "${root:?}/$d"
        fi
    done
fi

get_mumps() {
    # Same download as HOWTO_IMPLICIT.md, "Get the MUMPS source". The GNU MUMPS build keeps the
    # tarball in engine/extlib too, so it is downloaded once for both.
    local deps=$root/engine/extlib tgz sha
    tgz=$deps/MUMPS_5.5.1.tar.gz
    sha=1abff294fa47ee4cfd50dfd5c595942b72ebfcedce08142a75a99ab35014fa15
    if [ ! -f "$tgz" ]; then
        printf 'Downloading MUMPS 5.5.1 to engine/extlib ...\n'
        curl -fL --retry 3 -o "$tgz" https://ftp.mcs.anl.gov/pub/petsc/externalpackages/MUMPS_5.5.1.tar.gz
    fi
    printf '%s  %s\n' "$sha" "$tgz" | sha256sum -c --quiet -
    rm -rf "$deps/MUMPS_5.5.1"
    tar -xzf "$tgz" -C "$deps"
    [ -d "$deps/MUMPS_5.5.1/src" ]
}

step=0
total=4
[ "$intel" = yes ] && total=$((total + 1))

step=$((step + 1))
printf '\n=== [%d/%d] Double precision: starter_linux64_gf, engine_linux64_gf_ompi (MUMPS)\n' "$step" "$total"
bash "$root/build_linux_gf_mumps.sh" "$jobs"

step=$((step + 1))
printf '\n=== [%d/%d] Single precision: starter_linux64_gf_sp, engine_linux64_gf_sp, engine_linux64_gf_ompi_sp\n' "$step" "$total"
bash "$root/build_linux_gf_sp.sh" "$jobs"

step=$((step + 1))
printf '\n=== [%d/%d] Output converters: anim_to_vtk_linux64_gf, th_to_csv_linux64_gf\n' "$step" "$total"
bash "$root/tools/build_output_converters.sh"

step=$((step + 1))
printf '\n=== [%d/%d] OpenRadioss GUI: openradioss_gui/ (with inp2rad.py)\n' "$step" "$total"
python3 "$root/tools/stage_gui.py"

if [ "$intel" = yes ]; then
    step=$((step + 1))
    printf '\n=== [%d/%d] Double precision, Intel: starter_linux64_ifx, engine_linux64_ifx_impi (MUMPS)\n' "$step" "$total"
    if [ ! -d "$root/engine/extlib/MUMPS_5.5.1/src" ]; then
        get_mumps
    fi
    bash "$root/build_linux_mumps.sh" "$jobs"
elif [ "$intel" = skip ]; then
    printf '\n=== -no-intel: skipping starter_linux64_ifx and engine_linux64_ifx_impi.\n'
else
    printf '\n=== Intel oneAPI not found: skipping starter_linux64_ifx and engine_linux64_ifx_impi.\n'
fi

if [ "$smp" = 1 ]; then
    printf '\n=== [-smp] OpenMP-only double precision: engine_linux64_gf\n'
    bash "$root/build_linux.sh" "$jobs"
fi

if [ "$bundle" = 1 ]; then
    printf '\n=== [-bundle] Packaging bundles/OpenRadioss_linux64.zip\n'
    python3 "$root/Compiling_tools/script/make_bundle.py" --os linux64
fi

printf '\n=== Done. Executables in %s/exec:\n' "$root"
ls -1 "$root/exec"
printf '\nSelf-test (implicit, MUMPS): bash qa-tests/implicit/cantilever/run_cantilever.sh engine_linux64_gf_ompi\n'
printf 'GUI: bash openradioss_gui/OpenRadioss_gui.bash\n'
