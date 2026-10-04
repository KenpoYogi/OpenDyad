#!/usr/bin/env bash
# Build Starter and an Intel MPI Engine with MUMPS (implicit solver enabled), using Intel oneAPI.
# Needs MUMPS 5.5.1 extracted in engine/extlib/MUMPS_5.5.1.
# Loads oneAPI's setvars.sh itself when ifx is not already on PATH.
# This reader cannot generate /ALE/STRUCTURED_MESH; Starter rejects that keyword.
set -euo pipefail

root=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
jobs=${1:-$(getconf _NPROCESSORS_ONLN)}
if ! [[ "$jobs" =~ ^[1-9][0-9]*$ ]]; then
    printf 'Usage: bash build_linux_mumps.sh [positive number of build jobs]\n' >&2
    exit 1
fi

if [ ! -d "$root/engine/extlib/MUMPS_5.5.1/src" ]; then
    printf 'MUMPS 5.5.1 not found in engine/extlib/MUMPS_5.5.1\n' >&2
    exit 1
fi

if ! command -v ifx >/dev/null 2>&1; then
    oneapi_root=${ONEAPI_ROOT:-/opt/intel/oneapi}
    # setvars.sh is not written for "set -u", and would parse this script's arguments.
    set +u
    source "$oneapi_root/setvars.sh" --force >/dev/null
    set -u
fi
if ! command -v ifx >/dev/null 2>&1 || [ -z "${I_MPI_ROOT:-}" ]; then
    printf 'Intel Fortran or Intel MPI is not available. Install oneAPI or set ONEAPI_ROOT.\n' >&2
    exit 1
fi

python3 "$root/Compiling_tools/script/load_extlib.py"

cmake -S "$root/starter" -B "$root/starter/cbuild_linux64_ifx" \
    -Darch=linux64_ifx -Dprecision=dp -Ddebug=0 \
    -DEXEC_NAME=starter_linux64_ifx -DUSE_OPEN_READER=0 \
    -DHM_READER_LEGACY_INCLUDE_API=ON -DHM_READER_LEGACY_PART_API=ON \
    -DHM_READER_NO_STRUCTURED_ALE=ON \
    -DCMAKE_Fortran_COMPILER=ifx -DCMAKE_C_COMPILER=icx -DCMAKE_CXX_COMPILER=icpx
cmake --build "$root/starter/cbuild_linux64_ifx" --parallel "$jobs"

cmake -S "$root/engine" -B "$root/engine/cbuild_linux64_ifx_impi" \
    -Darch=linux64_ifx -Dprecision=dp -Ddebug=0 -DMPI=impi \
    -DEXEC_NAME=engine_linux64_ifx_impi \
    -DCMAKE_Fortran_COMPILER=ifx -DCMAKE_C_COMPILER=icx -DCMAKE_CXX_COMPILER=icpx
cmake --build "$root/engine/cbuild_linux64_ifx_impi" --parallel "$jobs"

printf '\nExecutables are in %s/exec\n' "$root"
printf 'Run the Engine with: mpiexec -n 1 engine_linux64_ifx_impi -i [Engine input file]\n'
