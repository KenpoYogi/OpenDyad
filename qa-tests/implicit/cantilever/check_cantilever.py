#!/usr/bin/env python3
"""Check an OpenRadioss run of the cantilever implicit / MUMPS test.

Usage:  python check_cantilever.py [run_dir]      (default: this script's folder)

Reads
  cantilever_0001.out   engine listing  -> proof that MUMPS ran and converged
  cantilever_*.sty      /OUTP/VECT/DISP -> nodal displacements (ASCII)
and compares DZ with the closed-form beam solution from make_cantilever.py.
Exit code 0 = PASS, 1 = FAIL, 2 = files missing.
"""
import re
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
sys.dont_write_bytecode = True  # keep __pycache__ out of the source tree
import make_cantilever as m  # noqa: E402  (parameters + analytic solution)

REL_TOL = 0.005         # accepted deviation, fraction of the tip deflection (FE gives -0.01 %)
RESIDUAL_MAX = 1.0e-6   # accepted relative residual ||K.u - F|| / ||F||

GOOD = {
    "implicit type = STATIC LINEAR": r"IMPLICIT TYPE\s*:.*STATIC LINEAR",
    "linear solver = DIRECT(MUMPS)": r"LINEAR SOLVER\s*:.*DIRECT\s*\(MUMPS\)",
    "BEGIN LINEAR STATIC IMPLICIT COMPUTATION": r"BEGIN LINEAR STATIC IMPLICIT COMPUTATION",
    "MUMPS banner (Entering DMUMPS)": r"Entering DMUMPS",
    "MUMPS factorization step": r"FACTORIZATION STEP",
    "MUMPS solve step": r"SOLVE & CHECK STEP",
    "NORMAL TERMINATION": r"NORMAL TERMINATION",
}
BAD = [
    r"Fatal error: MUMPS",
    r"MUMPS ERROR CODE",
    r"Warning: MUMPS(?! workspace too small\. Retry)",
    r"ERROR TERMINATION",
    r"STIFFNESS MATRIX IS NOT DEFINITE",
    r"STOPPED DUE TO",
    r"IS NOT AVAILABLE FOR STIFFNESS MATRIX",
]


def check_listing(path):
    ok = True
    text = path.read_text(errors="replace")
    print(f"--- {path.name}")
    for label, rx in GOOD.items():
        hit = re.search(rx, text) is not None
        ok &= hit
        print(f"  [{'ok' if hit else 'MISSING'}] {label}")
    for rx in BAD:
        for mt in re.finditer(rx, text):
            line = text[text.rfind("\n", 0, mt.start()) + 1:text.find("\n", mt.end())]
            print(f"  [BAD] {line.strip()}")
            ok = False
    # Engine's built-in fallback (imp_spmd.F): MUMPS memory cap too small, refactorize with more.
    n_retry = len(re.findall(r"Warning: MUMPS workspace too small\. Retry", text))
    if n_retry:
        print(f"  [info] MUMPS workspace retry x{n_retry} (engine fallback, not an error)")
    for mt in re.finditer(r"Entering DMUMPS\s+(\S+).*?JOB, N, NNZ =\s*(-?\d+)\s+(\d+)\s+(\d+)", text):
        print(f"  MUMPS {mt.group(1)}: JOB={mt.group(2)} N={mt.group(3)} NNZ={mt.group(4)}")
    res = re.findall(r"DIRECT SOLVER TERMINATED WITH RELATIVE \|\|R\|\|=\s*(\S+)", text)
    if not res:
        print("  [MISSING] DIRECT SOLVER TERMINATED WITH RELATIVE ||R||=  (needs /IMPL/PRINT/LINEAR/-1)")
        ok = False
    for r in res:
        try:
            v = float(r)
        except ValueError:
            v = 0.0 if re.match(r"0\.0+[-+]\d+$", r) else float("nan")
        good = v <= RESIDUAL_MAX
        ok &= good
        print(f"  [{'ok' if good else 'BAD'}] relative residual ||R|| = {r} (limit {RESIDUAL_MAX:g})")
    return ok


def read_sty(path):
    """Return (time, {node_id: (dx, dy, dz)}) from one OUTP .sty file."""
    lines = path.read_text(errors="replace").splitlines()
    time, disp, i = None, {}, 0
    while i < len(lines):
        ln = lines[i]
        if ln.startswith("/GLOBAL"):
            j = i + 1
            while j < len(lines) and (not lines[j].strip() or lines[j].startswith("#")):
                j += 1
            if j < len(lines):
                time = float(lines[j][:16])
        if ln.startswith("/NODAL") and "DISPLACEME" in ln:
            j, wi, wr = i + 1, 10, 20
            while j < len(lines) and not lines[j].startswith("# USRNOD"):
                f = re.search(r"\(I(\d+),1P3E(\d+)", lines[j])
                if f:
                    wi, wr = int(f.group(1)), int(f.group(2))
                j += 1
            j += 1
            while j < len(lines) and lines[j].strip() and lines[j][0] not in "/#":
                s = lines[j]
                disp[int(s[:wi])] = tuple(float(s[wi + k * wr:wi + (k + 1) * wr]) for k in range(3))
                j += 1
            i = j
            continue
        i += 1
    return time, disp


def check_displacements(run_dir):
    stys = sorted(run_dir.glob(f"{m.ROOT}_*.sty"))
    final = None
    for p in stys:
        t, d = read_sty(p)
        if d:
            final = (p, t, d)
    if final is None:
        print(f"--- no {m.ROOT}_*.sty with a DISPLACEMENT block in {run_dir}")
        return None
    p, t, d = final
    print(f"--- {p.name}  (time = {t})")
    w_tip = m.w_bernoulli(m.L)
    tol = REL_TOL * w_tip
    ok = True
    dx = m.L / m.NX
    rows = [(m.node_id(i, m.NY // 2), i * dx) for i in (m.NX // 4, m.NX // 2, 3 * m.NX // 4)]
    rows += [(m.node_id(m.NX, j), m.L) for j in range(m.NY + 1)]
    print(f"  {'node':>6} {'x':>7} {'DZ (FE)':>13} {'DZ (beam)':>11} {'diff %tip':>10}")
    for nid, x in rows:
        if nid not in d:
            print(f"  {nid:>6} missing from .sty")
            ok = False
            continue
        dz, ref = d[nid][2], m.w_bernoulli(x)
        good = abs(dz - ref) <= tol
        ok &= good
        print(f"  {nid:>6} {x:7.1f} {dz:13.6f} {ref:11.6f} {100 * (dz - ref) / w_tip:+9.3f}%"
              f"{'' if good else '   <-- FAIL'}")
    tip = [d[m.node_id(m.NX, j)][2] for j in range(m.NY + 1) if m.node_id(m.NX, j) in d]
    if tip:
        mean = sum(tip) / len(tip)
        print(f"  tip mean DZ = {mean:.6f} mm, expected {w_tip:.6f} mm "
              f"({100 * (mean - w_tip) / w_tip:+.3f} %), spread across width = {max(tip) - min(tip):.2e} mm")
    inplane = max(max(abs(v[0]), abs(v[1])) for v in d.values())
    good = inplane <= 1e-3 * w_tip
    ok &= good
    print(f"  [{'ok' if good else 'BAD'}] max |DX|,|DY| over all nodes = {inplane:.3e} mm (should be ~0)")
    return ok


def main():
    run_dir = Path(sys.argv[1]).resolve() if len(sys.argv) > 1 else HERE
    out = run_dir / f"{m.ROOT}_0001.out"
    if not out.exists():
        print(f"missing {out}")
        return 2
    ok_listing = check_listing(out)
    ok_disp = check_displacements(run_dir)
    if ok_disp is None:
        return 2
    verdict = ok_listing and ok_disp
    print(f"=== {'PASS' if verdict else 'FAIL'}  (listing {'ok' if ok_listing else 'FAIL'}, "
          f"displacements {'ok' if ok_disp else 'FAIL'}, tolerance {100 * REL_TOL:g} % of tip)")
    return 0 if verdict else 1


if __name__ == "__main__":
    sys.exit(main())
