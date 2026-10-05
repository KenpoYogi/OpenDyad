#!/usr/bin/env bash
# Build single-precision Starter and Engines (OpenMP only, and OpenMPI) with GCC.
# The implicit solver (MUMPS) needs double precision, so these Engines run explicit models only.
# OpenMPI 4 comes from OPENMPI_ROOT, default /usr/lib64/mpi/gcc/openmpi4
# (openSUSE openmpi4-devel).
# This reader cannot generate /ALE/STRUCTURED_MESH; Starter rejects that keyword.
set -euo pipefail

root=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
jobs=${1:-$(getconf _NPROCESSORS_ONLN)}
if ! [[ "$jobs" =~ ^[1-9][0-9]*$ ]]; then
    printf 'Usage: bash build_linux_gf_sp.sh [positive number of build jobs]\n' >&2
    exit 1
fi

ompi=${OPENMPI_ROOT:-/usr/lib64/mpi/gcc/openmpi4}
if [ ! -f "$ompi/include/mpif.h" ]; then
    printf 'OpenMPI not found in %s. Set OPENMPI_ROOT.\n' "$ompi" >&2
    exit 1
fi
ompi_lib=$ompi/lib64
[ -d "$ompi_lib" ] || ompi_lib=$ompi/lib

python3 "$root/Compiling_tools/script/load_extlib.py"

cmake -S "$root/starter" -B "$root/starter/cbuild_linux64_gf_sp" \
    -Darch=linux64_gf -Dprecision=sp -Ddebug=0 \
    -DEXEC_NAME=starter_linux64_gf_sp -DUSE_OPEN_READER=0 \
    -DHM_READER_LEGACY_INCLUDE_API=ON -DHM_READER_LEGACY_PART_API=ON \
    -DHM_READER_NO_STRUCTURED_ALE=ON
cmake --build "$root/starter/cbuild_linux64_gf_sp" --parallel "$jobs"

cmake -S "$root/engine" -B "$root/engine/cbuild_linux64_gf_sp" \
    -Darch=linux64_gf -Dprecision=sp -Ddebug=0 -DMPI=smp \
    -DEXEC_NAME=engine_linux64_gf_sp
cmake --build "$root/engine/cbuild_linux64_gf_sp" --parallel "$jobs"

cmake -S "$root/engine" -B "$root/engine/cbuild_linux64_gf_ompi_sp" \
    -Darch=linux64_gf -Dprecision=sp -Ddebug=0 -DMPI=ompi \
    -DEXEC_NAME=engine_linux64_gf_ompi_sp \
    -Dmpi_incdir="$ompi/include" -Dmpi_libdir="$ompi_lib"
cmake --build "$root/engine/cbuild_linux64_gf_ompi_sp" --parallel "$jobs"

printf '\nExecutables are in %s/exec\n' "$root"
