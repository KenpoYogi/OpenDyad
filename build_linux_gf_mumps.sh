#!/usr/bin/env bash
# Build Starter and a GNU + OpenMPI Engine with MUMPS (implicit solver enabled).
# First builds LAPACK 3.10.1, ScaLAPACK 2.2.0 and MUMPS 5.5.1 (double, PORD ordering)
# with OpenMPI's compiler wrappers into engine/extlib, downloading them once.
# OpenMPI 4 comes from OPENMPI_ROOT, default /usr/lib64/mpi/gcc/openmpi4
# (openSUSE openmpi4-devel). Its mpif90 must wrap gfortran.
# This reader cannot generate /ALE/STRUCTURED_MESH; Starter rejects that keyword.
set -euo pipefail

root=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
jobs=${1:-$(getconf _NPROCESSORS_ONLN)}
if ! [[ "$jobs" =~ ^[1-9][0-9]*$ ]]; then
    printf 'Usage: bash build_linux_gf_mumps.sh [positive number of build jobs]\n' >&2
    exit 1
fi

ompi=${OPENMPI_ROOT:-/usr/lib64/mpi/gcc/openmpi4}
if [ ! -x "$ompi/bin/mpif90" ]; then
    printf 'OpenMPI not found in %s. Set OPENMPI_ROOT.\n' "$ompi" >&2
    exit 1
fi
ompi_lib=$ompi/lib64
[ -d "$ompi_lib" ] || ompi_lib=$ompi/lib
export PATH=$ompi/bin:$PATH
export LD_LIBRARY_PATH=$ompi_lib:${LD_LIBRARY_PATH:-}

deps=$root/engine/extlib
lapack=$deps/lapack-3.10.1
scalapack=$deps/scalapack-2.2.0
mumps=$deps/MUMPS_5.5.1_gf_ompi
# GCC 10+ rejects argument mismatches in these libraries, and GCC 14+ rejects their old C idioms.
fflags="-O2 -fallow-argument-mismatch"
cflags="-O2 -std=gnu17 -Wno-error=implicit-function-declaration -Wno-error=implicit-int -Wno-error=incompatible-pointer-types -Wno-error=int-conversion"

fetch() {
    # fetch <url> <file> <sha256>
    [ -f "$deps/$2" ] || curl -fsSL -o "$deps/$2" "$1"
    printf '%s  %s\n' "$3" "$deps/$2" | sha256sum -c --quiet -
}

if [ ! -f "$lapack/libtmglib.a" ]; then
    fetch https://github.com/Reference-LAPACK/lapack/archive/refs/tags/v3.10.1.tar.gz lapack-3.10.1.tar.gz \
        cd005cd021f144d7d5f7f33c943942db9f03a28d110d6a3b80d718a295f7f714
    rm -rf "$lapack"
    tar -xzf "$deps/lapack-3.10.1.tar.gz" -C "$deps"
    cp "$lapack/make.inc.example" "$lapack/make.inc"
    make -C "$lapack" -j "$jobs" FC=gfortran CC=gcc FFLAGS="$fflags -frecursive" \
        FFLAGS_NOOPT="-O0 -frecursive -fallow-argument-mismatch" CFLAGS="$cflags" \
        blaslib lapacklib tmglib
fi

if [ ! -f "$scalapack/build/lib/libscalapack.a" ]; then
    fetch https://github.com/Reference-ScaLAPACK/scalapack/archive/refs/tags/v2.2.0.tar.gz scalapack-2.2.0.tar.gz \
        8862fc9673acf5f87a474aaa71cd74ae27e9bbeee475dbd7292cec5b8bcbdcf3
    rm -rf "$scalapack"
    tar -xzf "$deps/scalapack-2.2.0.tar.gz" -C "$deps"
    # ScaLAPACK asks for CMake 3.2, which CMake 4 refuses. The environment variable
    # also reaches the nested configure it runs to detect Fortran name mangling.
    CMAKE_POLICY_VERSION_MINIMUM=3.5 cmake -S "$scalapack" -B "$scalapack/build" \
        -DBUILD_SHARED_LIBS=OFF -DSCALAPACK_BUILD_TESTS=OFF \
        -DCMAKE_C_COMPILER=mpicc -DCMAKE_Fortran_COMPILER=mpif90 \
        -DCMAKE_C_FLAGS="$cflags" -DCMAKE_Fortran_FLAGS="$fflags" \
        -DLAPACK_LIBRARIES="$lapack/liblapack.a;$lapack/librefblas.a" \
        -DBLAS_LIBRARIES="$lapack/librefblas.a"
    CMAKE_POLICY_VERSION_MINIMUM=3.5 cmake --build "$scalapack/build" --parallel "$jobs"
fi

if [ ! -f "$mumps/lib/libdmumps.a" ]; then
    fetch https://ftp.mcs.anl.gov/pub/petsc/externalpackages/MUMPS_5.5.1.tar.gz MUMPS_5.5.1.tar.gz \
        1abff294fa47ee4cfd50dfd5c595942b72ebfcedce08142a75a99ab35014fa15
    # Kept apart from MUMPS_5.5.1, which the Intel build compiles in place.
    rm -rf "$mumps" "$deps/MUMPS_5.5.1_gf_tmp"
    mkdir "$deps/MUMPS_5.5.1_gf_tmp"
    tar -xzf "$deps/MUMPS_5.5.1.tar.gz" -C "$deps/MUMPS_5.5.1_gf_tmp"
    mv "$deps/MUMPS_5.5.1_gf_tmp/MUMPS_5.5.1" "$mumps"
    rmdir "$deps/MUMPS_5.5.1_gf_tmp"
    cat > "$mumps/Makefile.inc" <<EOF
LPORDDIR = \$(topdir)/PORD/lib/
IPORD    = -I\$(topdir)/PORD/include/
LPORD    = -L\$(LPORDDIR) -lpord
ORDERINGSF  = -Dpord
ORDERINGSC  = \$(ORDERINGSF)
LORDERINGS = \$(LPORD)
IORDERINGSF =
IORDERINGSC = \$(IPORD)
PLAT    =
LIBEXT  = .a
# MUMPS writes \$(OUTC)\$@ and \$(AR)\$@, so these need a trailing space.
empty   :=
space   := \$(empty) \$(empty)
OUTC    = -o\$(space)
OUTF    = -o\$(space)
RM      = /bin/rm -f
CC      = mpicc
FC      = mpif90
FL      = mpif90
AR      = ar vr\$(space)
RANLIB  = ranlib
LAPACK  = $lapack/liblapack.a
SCALAP  = $scalapack/build/lib/libscalapack.a
LIBBLAS = $lapack/librefblas.a
INCPAR  =
LIBPAR  = \$(SCALAP) \$(LAPACK)
INCSEQ  = -I\$(topdir)/libseq
LIBSEQ  = \$(LAPACK) -L\$(topdir)/libseq -lmpiseq
LIBOTHERS = -lpthread
CDEFS   = -DAdd_
OPTF    = $fflags
OPTC    = $cflags -I.
OPTL    = -O2
INCS    = \$(INCPAR)
LIBS    = \$(LIBPAR)
LIBSEQNEEDED =
EOF
    make -C "$mumps" -j "$jobs" d
fi

python3 "$root/Compiling_tools/script/load_extlib.py"

cmake -S "$root/starter" -B "$root/starter/cbuild_linux64_gf" \
    -Darch=linux64_gf -Dprecision=dp -Ddebug=0 \
    -DEXEC_NAME=starter_linux64_gf -DUSE_OPEN_READER=0 \
    -DHM_READER_LEGACY_INCLUDE_API=ON -DHM_READER_LEGACY_PART_API=ON \
    -DHM_READER_NO_STRUCTURED_ALE=ON
cmake --build "$root/starter/cbuild_linux64_gf" --parallel "$jobs"

cmake -S "$root/engine" -B "$root/engine/cbuild_linux64_gf_ompi" \
    -Darch=linux64_gf -Dprecision=dp -Ddebug=0 -DMPI=ompi \
    -DEXEC_NAME=engine_linux64_gf_ompi \
    -Dmpi_incdir="$ompi/include" -Dmpi_libdir="$ompi_lib" \
    -Dmumps_root="$mumps" -Dscalapack_root="$scalapack/build/lib" -Dlapack_root="$lapack"
cmake --build "$root/engine/cbuild_linux64_gf_ompi" --parallel "$jobs"

printf '\nExecutables are in %s/exec\n' "$root"
printf 'Run the Engine with: mpiexec -n 1 engine_linux64_gf_ompi -i [Engine input file]\n'
printf '(with %s/bin on PATH and %s on LD_LIBRARY_PATH)\n' "$ompi" "$ompi_lib"
