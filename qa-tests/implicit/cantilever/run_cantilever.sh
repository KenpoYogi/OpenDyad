#!/usr/bin/env bash
# Runs the implicit cantilever check: Starter, Engine (MPI), then check_cantilever.py.
# Usage: bash run_cantilever.sh [engine] [ranks]
# Uses OPENRADIOSS_PATH when set, otherwise this repo.
# Intel MPI engines (*_impi) load oneAPI when needed; OpenMPI engines (*_ompi) use
# OPENMPI_ROOT, default /usr/lib64/mpi/gcc/openmpi4 (openSUSE openmpi4-devel).
set -euo pipefail

here=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
engine=${1:-engine_linux64_ifx_impi}
np=${2:-1}
root=${OPENRADIOSS_PATH:-$(CDPATH= cd -- "$here/../../.." && pwd)}

case $engine in
    *_gf*) starter=starter_linux64_gf ;;
    *)     starter=starter_linux64_ifx ;;
esac
[ -x "$root/exec/$starter" ] || starter=$(basename "$(ls "$root"/exec/starter_linux64* | head -n 1)")

case $engine in
    *_impi*)
        if ! mpiexec --version 2>/dev/null | grep -q Intel; then
            set +u
            source "${ONEAPI_ROOT:-/opt/intel/oneapi}/setvars.sh" --force >/dev/null
            set -u
        fi
        ;;
    *_ompi*)
        ompi=${OPENMPI_ROOT:-/usr/lib64/mpi/gcc/openmpi4}
        if [ -d "$ompi/bin" ]; then
            export PATH=$ompi/bin:$PATH
            export LD_LIBRARY_PATH=$ompi/lib64:$ompi/lib:${LD_LIBRARY_PATH:-}
        fi
        ;;
esac

export RAD_CFG_PATH=$root/hm_cfg_files
export RAD_H3D_PATH=$root/extlib/h3d/lib/linux64
export LD_LIBRARY_PATH=$root/extlib/hm_reader/linux64:${LD_LIBRARY_PATH:-}
export OMP_STACKSIZE=${OMP_STACKSIZE:-400m}
export OMP_NUM_THREADS=${OMP_NUM_THREADS:-1}

run=${TMPDIR:-/tmp}/openradioss_cantilever_${engine}_np${np}
rm -rf "$run"
mkdir -p "$run"
cp "$here/cantilever_0000.rad" "$here/cantilever_0001.rad" "$run/"
cd "$run"
starter_rc=0
"$root/exec/$starter" -i cantilever_0000.rad -np "$np" > starter_console.txt 2>&1 || starter_rc=$?
engine_rc=0
mpiexec -n "$np" "$root/exec/$engine" -i cantilever_0001.rad > engine_console.txt 2>&1 || engine_rc=$?
printf 'Run folder: %s (%s + %s, %s ranks)\n' "$run" "$starter" "$engine" "$np"
[ "$starter_rc" -eq 0 ] || printf 'Starter exit code %s, see starter_console.txt\n' "$starter_rc"
[ "$engine_rc" -eq 0 ] || printf 'Engine exit code %s, see engine_console.txt\n' "$engine_rc"
python3 "$here/check_cantilever.py" "$run"
