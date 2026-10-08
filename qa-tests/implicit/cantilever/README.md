# Implicit linear static check: cantilever strip (MUMPS)

A small model with a textbook answer. It checks that an Engine built with MUMPS
runs a linear static implicit solve through the MUMPS direct solver and gets the
right deflection. The repo's miniqa suite has no implicit cases; this one is run
on its own, not through ctest.

## Model

| Item | Value |
|---|---|
| Units | Mg, mm, s (force N, stress MPa) |
| Geometry | Flat strip in XY: L = 100, b = 10, t = 1 mm |
| Mesh | 40 x 4 shells, Ishell 24 (QEPH), 205 nodes |
| Material | `/MAT/ELAST`, E = 200000 MPa, nu = 0 |
| Supports | All 6 DOF fixed on the 5 nodes at x = 0 |
| Load | P = 1 N in +Z, spread over the 5 tip nodes at x = 100 |

With nu = 0 the strip behaves exactly like a beam, so the expected tip
deflection is P·L³/(3EI) = 1e6 / (3 · 200000 · 10/12) = **2.000 mm**. Shear adds
0.006 %. The checker also compares nodes at x = 25, 50 and 75 mm.

Tip DZ = 1.999808 mm (−0.010 %), relative residual about 1.2e-9 to 1.3e-9, at 1
and 2 MPI ranks, for all three builds:
- Intel 2026.1 + Intel MPI on Windows
- Intel 2026.1 + Intel MPI on Linux
- GCC 16 + OpenMPI 4.1 on Linux

## Run it

Build first, with `build_windows_all.bat` or `build_linux_all.sh` in the repo
root (the default full builds), or with one of the individual scripts:
- `build_windows_mumps.bat`
- `build_linux_mumps.sh` (Intel)
- `build_linux_gf_mumps.sh` (GNU + OpenMPI)

The run scripts copy the decks to a scratch folder, run Starter and Engine
there, and then run the checker. They also work from an unzipped release bundle.

```bat
qa-tests\implicit\cantilever\run_cantilever.bat [engine] [ranks]
```

```bash
bash qa-tests/implicit/cantilever/run_cantilever.sh [engine] [ranks]
```

- The default engine is `engine_win64_impi` on Windows and `engine_linux64_ifx_impi` on Linux. On Linux you can also pass `engine_linux64_gf_ompi`.
- Ranks defaults to 1.
- The scripts use `OPENRADIOSS_PATH` if it is set, otherwise the repo root. They load oneAPI when `mpiexec` is not on `PATH`.

The checker prints PASS or FAIL and returns 0 for PASS, 1 for FAIL and 2 when
output files are missing. To check a run folder yourself:
`python check_cantilever.py <run folder>`.

The tolerance is 0.5 % of the tip deflection. To change the mesh or the load,
edit `make_cantilever.py` and run it again; the checker reads the same values.

## Keywords that matter

- **Starter:** `/IMPLICIT` (no data lines).
- **`/IMPL/LINEAR`:** one step from 0 to the `/RUN` end time. Static is the default when there is no `/IMPL/DYNA`, and `/IMPL/STATIC` does not exist; an unknown `/IMPL` key stops the Engine.
- **`/IMPL/SOLVER/2`:** selects MUMPS (`lectur.F` maps it to the direct MUMPS path). Its data line must carry all four values.
- **`/IMPL/PRINT/LINEAR/-1`:** prints `DIRECT SOLVER TERMINATED WITH RELATIVE ||R||=`.
- **`/IMPL/MUMPS/MSGLV/2`:** copies MUMPS's own messages (`Entering DMUMPS 5.5.1 ...`) into the listing.
- **Shells:** Ishell 24 and 12 have native implicit stiffness. Ishell 1 to 4 only give a "not available for stiffness matrix" warning.

The tip displacement is read from `cantilever_0001.sty` (`/OUTP/VECT/DISP`).
Time history is in `cantileverT01` for nodes 53, 103, 153 and 201 to 205.
Convert it to CSV with `th_to_csv`, and convert the animation files
(`cantileverA001`, ...) to VTK with `anim_to_vtk`. The sources are in `tools/`;
build them with `tools/build_output_converters.*`.

## Reading the listing

A good run's `cantilever_0001.out` contains:
- `STATIC LINEAR` and `DIRECT(MUMPS)`
- `Entering DMUMPS` with N = 1200 (200 free nodes × 6 DOF)
- the analysis, factorization and solve steps
- the residual line
- `NORMAL TERMINATION`

One line is normal: on the first factorization, `Warning: MUMPS workspace too
small. Retry`. This is the Engine's built-in fallback (`imp_spmd.F`), which
retries with more memory. The checker reports it as info, not as a failure.

Signs of a broken build:
- `Fatal error: MUMPS required` on the console: the Engine was compiled without `-DMUMPS5`.
- A crash right after `SOLUTION PHASE`. See the `DIM_KINMAX` fix in `engine/source/implicit/ind_glob_k.F`.
