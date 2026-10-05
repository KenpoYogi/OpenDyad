# Plan: restore buckling (`/IMPL/BUCKL`) and modal analysis (`/EIG`)

Status: **plan only; no code has been changed.** Written 2026-10-04 against
`main` at `66ac373`.

## Summary

- `/EIG` (natural frequencies and mode shapes) and `/IMPL/BUCKL` (buckling
  loads) cannot be enabled by defining the `DNC` macro. The eigen-solver core
  was never released: it lived in upstream's closed `engine/com/` tree, and
  only stubs are open source.
- `DNC` ("do not compile") guards every feature missing from the open-source
  release, not only the eigen solver. Defining it globally would fail to link.
- Most of the surrounding code is present: input reading, the `K` and
  geometric stiffness (`KG`) assembly, the MUMPS setup and the output of
  results.
- The missing pieces were built on ARPACK. The maintained open-source successor,
  **ARPACK-ng** (BSD-3), fits the existing interfaces directly.
- Order of work:
  1. A plumbing spike.
  2. **`/IMPL/BUCKL`**: one missing routine, with a working driver around it.
  3. **`/EIG`**: a new driver modelled on the buckling one.
  4. MPI support for both.

## 1. Findings

### What is missing

| Routine | Role | Where it is referenced | Status |
| --- | --- | --- | --- |
| `IMP_EIGSOL` | `/EIG` driver | [`engine/source/engine/resol.F`](../engine/source/engine/resol.F), the `NEIG>0` block near line 2426, inside `#ifdef DNC` | not defined anywhere |
| `EIGBUCKP` | ARPACK solve for buckling | [`engine/source/implicit/imp_buck.F`](../engine/source/implicit/imp_buck.F):672, inside `#ifdef DNC` | not defined anywhere |
| `EIG`, `EIG1`, `EIGP`, `EIGCOND`, `KLDIM` | `/EIG` internals, including condensation | `engine/stub/*.F`, compiled only when `DNC` is **not** defined | stubs: `EIG` and `EIGCOND` call `ARRET(5)`; `EIG1` and `EIGP` return |
| `engine/com/eig/*.F` | upstream's real implementation | the `COM` branch of `engine/CMakeLists.txt` | not in this repository |

[OpenCourant](https://github.com/OpenCourant/OpenCourant) was checked on
2026-10-04. It has the same stubs and no implementation, so there is no
community code to reuse.

### What `DNC` guards

The macro guards 60 blocks in 28 files. Only three of them matter here. The
rest belong to features whose code is also missing:

- MDS user-material libraries (`MDS_*`)
- ABF output pipes (`ABF*`)
- MADYMO coupling (`*_MADCPL`)
- parts of the rad2rad coupling

**Decision: never define `DNC`.** Use a new macro for the three eigen call
sites only.

### What is present and reusable

| Piece | Location |
| --- | --- |
| `/EIG` input: read by the Starter and written to the restart file | `starter/source/general_controls/computation/hm_read_eig.F`, `starter/source/restart/ddsplit/c_eig.F`, `wrrest.F`; `eigcom.inc`, `eig_mod.F` |
| `/EIG` card definition | `hm_cfg_files/config/CFG/radioss110/BODY/eig.cfg` |
| `/IMPL/BUCKL` input | [`engine/source/input/freimpl.F`](../engine/source/input/freimpl.F):468-486 (`EMIN_B EMAX_B NBUCK MSGL_B MAXSET_B SHIFT_B`; defaults `BNITER=300`, `BINCV=4`, `BMAXNCV=16`) |
| Solver choice for buckling: forces the MUMPS direct solver (`ISOLV=3`) | [`engine/source/input/lectur.F`](../engine/source/input/lectur.F):3298 |
| Buckling driver `IMP_BUCK`: assembles `K` and `KG`, builds `K − σ·KG`, fills the MUMPS structure, prints critical loads, writes mode shapes to ANIM and OUTP | [`imp_buck.F`](../engine/source/implicit/imp_buck.F) (999 lines). Its call in `resol.F` (near line 8819) is under `#if defined(MUMPS5) && defined(DNC)`, so **it has never run in an open-source build**. |
| Sparse symmetric `K·x` (lower-triangle storage `DIAG_K`, `LT_K`, `IADK`, `JDIK`) | `MAV_LT`, and `MAV_LTP` for MPI, in `engine/source/implicit/produt_v.F` |
| Geometric stiffness terms | Solids: `s4kgeo3`, `s8zkgeo3`, `s10kgeo3`, `s20kgeo3`. Thick shells: `s6ckgeo3`. Shells: `cbake3`, `czke3`. Beam: `pke3`. Truss: `tke3`. Springs: `r4ke3`, `r13ke3`. Interface type 24: `i24ke3`. Coverage per element still has to be confirmed. |
| MUMPS factorization and solves | our MPI + MUMPS builds |

### The interfaces to implement

`EIGBUCKP` is called by `IMP_BUCK` with ARPACK's parameter set. The names
match `dsaupd`/`dseupd`:

```fortran
CALL EIGBUCKP(N,      NEV,    NCV,    WHICH,  INFO,
              MAXN,   MAXNEV, MAXNCV, LDV,    ISHFTS,
              MAXITR, MODE,   TOL,    IADK,   JDIK,
              DIAG_K, LT_K,   DIAG_KG, LT_KG, EIG,
              VECT,   IPRI,   SHIFT,  MUMPS_PAR, CDDLP,
              NDDL,   MULTD)
```

`IMP_BUCK` sets these values (`imp_buck.F`:511-536):

- `NEV=NBUCK` and `NCV=BINCV·NEV`
- `WHICH='LM'`, `ISHFTS=1`, `MODE=4` (ARPACK's buckling mode) and `TOL=0`
- `SHIFT` from `/IMPL/BUCKL`
- `VECT(LDV,MAXNCV)` and `EIG(MAXNCV,2)`

It then builds the operator `DIAG_OP = DIAG_K − SHIFT·DIAG_KG` (lines
546-551) and loads it into `MUMPS_PAR` (lines 640-660). It expects the
critical-load factors back in `EIG(1:NBUCK,1)` and the eigenvectors in
`VECT(:,1:NBUCK)` (line 810).

`IMP_EIGSOL`'s argument list is the call in `resol.F` (near line 2426). The
`/EIG` parameters arrive in `EIGIPM`, `EIGRPM` and `EIGIBUF`. This is the
layout the Starter writes (`hm_read_eig.F`), to be confirmed in Phase 2:

| `EIGIPM(IAD+k)` | Meaning | `EIGRPM(IADF+k)` | Meaning |
| --- | --- | --- | --- |
| +0 | card ID | +0 | `TOL` |
| +1 | type | +1 | 0 |
| +2, +3 | translation and rotation codes | +2 | shift, stored as (2π·`Freqmin`)² |
| +4 | `Nmod` (number of modes) | +3 | `Cutfreq` |
| +5 | `Incv` | | |
| +6 | `Niter` | | |
| +7 | `Ipri` | | |
| +8 | `Nbloc` | | |
| +9, +10 | node counts | | |
| +11 | address in `EIGIBUF` | | |
| +12, +13 | additional-modes file data | | |
| +14 | `IPRSP` | | |
| +16 | `Imls` (multi-level condensation) | | |

## 2. Eigen solver choice

| Option | License | Verdict |
| --- | --- | --- |
| **[ARPACK-ng](https://github.com/opencollab/arpack-ng)** 3.9.1 (latest tag; packaged on openSUSE as `arpack-ng-devel`) | BSD-3 | **Use it.** Upstream used ARPACK: the `EIGBUCKP` arguments match, `MODE=4` is ARPACK's buckling mode, and `eig.cfg` mentions "ARPACK" and "Lanczos". It is Fortran, calls back for each matrix solve (so it works with MUMPS), and PARPACK covers MPI. |
| MKL FEAST (Intel's extended eigensolver) | Intel | Intel builds only; the GNU build has no MKL. |
| SLEPc | BSD-2 | Capable, but requires PETSc: too heavy for this. |
| PRIMME, Spectra | BSD-3, MPL-2 | Would need C or C++ glue code, with no advantage over ARPACK here. |
| Dense LAPACK `dsygvx` | BSD | Not for real models. Use it as an exact reference in tests on tiny models. |

**Integration approach:** fetch the ARPACK-ng sources once, verified by
checksum, and compile the needed double-precision files into the MPI + MUMPS
Engines, the same way MUMPS is integrated.

- The Intel builds take BLAS and LAPACK from MKL. The GNU build uses the
  LAPACK 3.10.1 that `build_linux_gf_mumps.sh` already builds.
- One code path then works on Windows and Linux, and the bundles stay
  self-contained.
- Linking the system `arpack-ng-devel` package is an alternative on Linux
  only.

## 3. Design

### 3.1 Build integration

- **New macro `ARPACK`.** Defined only when the ARPACK-ng sources are present
  and the build is double precision with MUMPS.
- **Guards:** change these three from `DNC` to `ARPACK`:
  - the `IMP_EIGSOL` call in `resol.F` (near line 2426)
  - the `IMP_BUCK` call in `resol.F` (near line 8819): `MUMPS5 && DNC` becomes `MUMPS5 && ARPACK`
  - the `EIGBUCKP` call in `imp_buck.F`:672
- **Stubs:** without `ARPACK`, the call sites keep a clear stop, in the style
  of the existing "MUMPS required" message: "Fatal error: /EIG and /IMPL/BUCKL
  need an Engine built with ARPACK".
- **Where the build changes go:**
  - Windows: `engine/CMakeLists.txt` and `cmake_win64.txt`, with source globbing like the MUMPS block.
  - Linux Intel: `cmake_linux64_ifx.txt`.
  - Linux GNU: `cmake_linux64_gf.txt`, plus a step in `build_linux_gf_mumps.sh`.
- **Licensing:** add ARPACK-ng's license to `make_bundle.py`'s licenses
  folder.

### 3.2 `EIGBUCKP` (buckling)

ARPACK mode 4 solves `K·x = λ·KG·x` with the shift-invert operator
`OP = (K − σ·KG)⁻¹·K` and `B = K`.

- **Before the loop:** MUMPS analysis and factorization of `K − σ·KG`, which
  `IMP_BUCK` has already loaded into `MUMPS_PAR`.
- **ARPACK request "apply OP" (`IDO = −1` or `1`):** compute `K·x` with
  `MAV_LT`, then solve with the stored factorization (MUMPS `JOB=3`).
- **ARPACK request "apply B" (`IDO = 2`):** `y = K·x` with `MAV_LT`.
- **After convergence:** `dseupd` returns the eigenvalues and Ritz vectors.
  Copy the critical-load factors into `EIG(:,1)` and the vectors into
  `VECT(:,1:NBUCK)`. Respect `EMIN_B` and `EMAX_B`, and the sign convention
  `IMP_BUCK` prints.
- **One MPI rank first.** On more ranks, stop with a clear message until
  Phase 3.

### 3.3 `IMP_EIGSOL` (modal)

Use `IMP_BUCK` as the template: the same `K` assembly, boundary conditions,
kinematic conditions, MUMPS setup and ANIM/OUTP writing of modes.

- **Mass:** replace `KG` with the lumped mass, which is diagonal in DOF space:
  the nodal mass `MS` and the rotational inertia `IN`.
- **Solve:** ARPACK mode 3 (shift-invert) with `OP = (K − σ·M)⁻¹·M` and
  `B = M`, where `B` is a diagonal scale.
- **Shift:**
  - If `Freqmin` is given, σ = (2π·`Freqmin`)², which the Starter already stores in `EIGRPM`.
  - Otherwise use a small negative σ, so that free-free models stay solvable: their six rigid-body modes make `K` singular.
- **Results:**
  - **Frequencies:** f = √λ / 2π. Print them to the listing with the mode number, f and ω.
  - **Normalization:** `Inorm` selects mass or maximum normalization.
  - **Mode shapes:** write them to the animation files.
  - **Cut-off:** drop modes above `Cutfreq`.
- **First version:** `Nmod`, `Freqmin`, `Cutfreq`, `Incv`, `Niter`, `Tol`,
  `Ipri`, `Inorm`, the mode node group and the eigen boundary-condition group.
- **Deferred:** `Imls` (multi-level condensation), `Ifile` (additional modes
  file) and `Nbloc`. These likely correspond to `EIG1`, `EIGCOND` and
  flexible-body (`/FXBODY`) generation.

## 4. Phases

Estimates are rough and assume Phase 0 finds no major breakage.

### Phase 0: plumbing spike (about 1 day)

1. Add ARPACK-ng 3.9.1 to the builds (§3.1), and confirm that `dsaupd` and `dseupd` link in all three MPI + MUMPS Engines.
2. Add the `ARPACK` macro and change the three guards. Temporarily make `EIGBUCKP` a routine that does nothing.
3. Run a small `/IMPL/BUCKL` deck. Check that `IMP_BUCK` assembles `K` and `KG`, builds the MUMPS input and reaches its output code without crashing.
4. Confirm which elements contribute `KG`, and how the buckling step follows the static step: `IBUCK` and `NBUCK` handling in `resol.F`, near lines 8806-8820.

**Exit criterion / decision point:** `IMP_BUCK` runs end to end with the
stub. If it needs extensive repair, re-plan before Phase 1.

### Phase 1: `/IMPL/BUCKL`, one MPI rank (about 2-4 days)

1. Write `EIGBUCKP` (§3.2), placed in `engine/source/implicit/`.
2. Add tests under `qa-tests/implicit/` with a generator and a checker, following the cantilever test (§5).
3. Update the HOWTO, the README section and the troubleshooting table.

**Exit criterion:** the first three critical loads of the column tests are
within 2 % of Euler theory, on all three Engines, Windows and Linux.

### Phase 2: `/EIG`, one MPI rank (about 1-2 weeks)

1. Confirm the `EIGIPM`/`EIGRPM`/`EIGIBUF` layout against `hm_read_eig.F` and `c_eig.F`.
2. Write `IMP_EIGSOL` (§3.3), including the listing table and the animation output.
3. Write tests for clamped-free and free-free modes (§5), plus a dense-LAPACK comparison on a tiny model.

**Exit criterion:** the first three frequencies are within 1 % of beam theory
for the out-of-plane modes. A free-free model reports exactly 6 near-zero
modes before the first elastic one.

### Phase 3: MPI, PARPACK (about 1 week)

1. Switch both drivers to `pdsaupd` and `pdseupd`, with distributed vectors and `MAV_LTP`. `MULTD`, the DOF multiplicity across domains already passed to `EIGBUCKP`, weights the inner products.
2. Remove the one-rank stops.

**Exit criterion:** results on 2 and 4 ranks match 1 rank to solver tolerance.

### Phase 4: optional, only if needed

- `/EIG` condensation (`Imls`), the additional-modes file (`Ifile`) and `/FXBODY` mode generation.

## 5. Validation cases

All cases reuse the strip from `qa-tests/implicit/cantilever`: L = 100 mm,
b = 10 mm, t = 1 mm, E = 200000 MPa, ν = 0, ρ = 7.85e-9 Mg/mm³, Ishell 24.
Reference values were computed with Euler-Bernoulli beam theory.

**Modal (`/EIG`):**

| Case | Mode 1 | Mode 2 | Mode 3 | Tolerance |
| --- | --- | --- | --- | --- |
| Clamped-free, out-of-plane bending | 81.54 Hz | 510.99 Hz | 1430.79 Hz | 1 % |
| Clamped-free, in-plane bending | 815.4 Hz | — | — | 3 % (shear effects at L/b = 10) |
| Free-free, out-of-plane bending | 6 rigid-body modes ≈ 0 Hz, then 518.85 Hz | 1430.22 Hz | — | 1 % |

**Buckling (`/IMPL/BUCKL`, axial compression):** apply P = 1 N, so the
reported load factor equals P_cr.

| Supports | P_cr = π²EI/(K·L)² | Higher modes | Tolerance |
| --- | --- | --- | --- |
| Clamped-free (K = 2) | 41.12 N | — | 2 % |
| Pinned-pinned (K = 1) | 164.49 N | n²·P_cr | 2 % |
| Clamped-clamped (K = 0.5) | 657.97 N | — | 2 % |

**Cross-checks:**
- A tiny model (2 to 4 elements) compared with dense LAPACK `dsygvx` on the same assembled `K` and `M`, or `K` and `KG`.
- Results identical on Windows and Linux, and on 1 versus N ranks (Phase 3).
- Single-precision Engines keep refusing implicit decks with the existing message.

## 6. Risks

| Risk | Mitigation |
| --- | --- |
| `IMP_BUCK` has never run in an open-source build and may have bit-rot or compiler problems, like the earlier `DIM_KINMAX` miscompile | Phase 0 runs it first. Compare debug and optimized builds. |
| The missing routines' conventions must be inferred: `KG` sign, `EIG` and `VECT` layout, `CDDLP` and `MULTD` | Read the callers. Check against analytic cases and the dense-LAPACK reference. |
| `KG` may be missing or wrong for some element formulations | Confirm per element in Phase 0. Restrict the documented support to verified elements. |
| ARPACK convergence: repeated eigenvalues, rigid-body modes, a poor shift | Shift strategy (§3.3); expose `Incv`, `Niter` and `Tol`; test free-free. |
| Lumped mass lowers higher frequencies slightly | Tolerances per mode; mesh refinement in tests. |
| Scope creep toward full `/EIG` parity (condensation, flexible bodies) | Phase 4 is optional and separate. |

## 7. Questions to settle before starting

1. Which matters more, buckling or modal? The plan does buckling first because it is smaller.
2. Is one MPI rank enough at first, or is MPI (Phase 3) needed early?
3. Are condensation and `/FXBODY` mode files (Phase 4) needed at all?
4. Vendor ARPACK-ng sources (recommended, works on both OSes), or link the system package on Linux?

## 8. Out of scope

- Single-precision eigen analysis. Implicit stays double precision only.
- Eigen analysis on the SMP Engines. ARPACK with MUMPS needs the MPI + MUMPS Engines.
- Defining `DNC` or restoring any of the other features it guards.
