#!/usr/bin/env python3
"""Package built OpenRadioss executables into a release-style zip.

The layout follows the upstream delivery workflow
(.github/workflows/delivery_stable_ci.yaml), plus the implicit self-test:

  OpenRadioss/
    exec/                               Starter and Engine executables
    extlib/h3d/lib/<os>/
    extlib/hm_reader/<os>/
    extlib/intelOneAPI_runtime/win64/   Windows only: OpenMP and MKL DLLs
    hm_cfg_files/
    licenses/
    qa-tests/implicit/cantilever/       Implicit MUMPS check (run_cantilever.*)
    COPYRIGHT.md, LICENSE.md, README.txt

Usage: python make_bundle.py --os win64|linux64 [--out DIR]
Run after build_windows_mumps.bat + build_windows_compat.bat (win64), or after
build_linux_mumps.sh + build_linux_gf_mumps.sh + build_linux.sh (linux64).
"""
import argparse
import datetime
import glob
import os
import shutil
import subprocess
import sys
import tempfile
import zipfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
TOP = "OpenRadioss"

EXECUTABLES = {
    "win64": ["starter_win64.exe", "engine_win64.exe", "engine_win64_impi.exe"],
    "linux64": ["starter_linux64_gf", "engine_linux64_gf", "engine_linux64_gf_ompi",
                "engine_linux64_ifx_impi"],
}
# Intel runtime shipped in extlib/intelOneAPI_runtime/<os>, as (oneAPI component, subfolder, patterns).
# MKL loads its CPU-specific kernels at run time, so they do not show up as imports.
# On Linux, -static-intel still leaves libirng.so dynamic in the ifx Engine; it needs libintlc.so.5.
RUNTIME = {
    "win64": [("compiler", "bin", ["libiomp5md.dll"]),
              ("mkl", "bin", ["mkl_core.*.dll", "mkl_intel_thread.*.dll", "mkl_def.*.dll",
                              "mkl_mc3.*.dll", "mkl_avx*.dll", "mkl_vml_*.dll"])],
    "linux64": [("compiler", "lib", ["libirng.so", "libintlc.so.5"])],
}


def oneapi_root(osname):
    default = (Path(os.environ.get("ProgramFiles(x86)", r"C:\Program Files (x86)")) / "Intel" / "oneAPI"
               if osname == "win64" else Path("/opt/intel/oneapi"))
    return Path(os.environ.get("ONEAPI_ROOT", default))


def run(cmd):
    try:
        return subprocess.run(cmd, capture_output=True, text=True, check=True).stdout.strip()
    except (OSError, subprocess.CalledProcessError):
        return ""


def copy(src, dst):
    dst.parent.mkdir(parents=True, exist_ok=True)
    if src.is_dir():
        shutil.copytree(src, dst, dirs_exist_ok=True)
    else:
        shutil.copy2(src, dst)


def stage_licenses(osname, stage, oneapi):
    lic = stage / "licenses"
    for f in (ROOT / "extlib" / "license").iterdir():
        copy(f, lic / f.name)
    mumps = ROOT / "engine" / "extlib" / "MUMPS_5.5.1" / "LICENSE"
    if osname == "linux64" and not mumps.exists():
        mumps = ROOT / "engine" / "extlib" / "MUMPS_5.5.1_gf_ompi" / "LICENSE"
    copy(mumps, lic / "mumps_license.txt")
    if osname == "linux64":
        copy(ROOT / "engine" / "extlib" / "scalapack-2.2.0" / "LICENSE", lic / "scalapack_license.txt")
    # Intel runtime and statically linked MKL / OpenMP.
    for d in (oneapi / "mkl" / "latest" / "share" / "doc" / "mkl" / "licensing",
              oneapi / "mkl" / "latest" / "licensing"):
        for name in ("license.rtf", "license.txt", "third-party-programs.txt",
                     "third-party-programs-openmp.txt"):
            if (d / name).exists():
                copy(d / name, lic / f"intel_mkl_{name}")


def readme(osname, exes, oneapi):
    commit = run(["git", "-C", str(ROOT), "rev-parse", "--short", "HEAD"]) or "unknown"
    branch = run(["git", "-C", str(ROOT), "rev-parse", "--abbrev-ref", "HEAD"]) or "unknown"
    dirty = " (with uncommitted changes)" if run(["git", "-C", str(ROOT), "status", "--porcelain",
                                                   "--untracked-files=no"]) else ""
    versions = []
    for comp in ("compiler", "mkl", "mpi"):
        latest = oneapi / comp / "latest"
        if latest.exists():
            versions.append(f"{comp} {latest.resolve().name}")
    tools = [f"Intel oneAPI: {', '.join(versions)}"] if versions else []
    if osname == "linux64":
        gf = run(["gfortran", "-dumpfullversion"])
        if gf:
            tools.append(f"GNU gfortran {gf}")
        ompi = run(["ompi_info", "--version"]).splitlines()
        if ompi:
            tools.append(ompi[0])
    lines = [
        "OpenRadioss double-precision build with the MUMPS 5.5.1 implicit solver",
        "",
        f"Built {datetime.date.today().isoformat()} from branch {branch}, commit {commit}{dirty}.",
        *[f"  {t}" for t in tools],
        "",
        "exec/ contains:",
    ]
    desc = {
        "starter_win64.exe": "Starter",
        "engine_win64.exe": "Engine, OpenMP only (explicit; no implicit)",
        "engine_win64_impi.exe": "Engine, Intel MPI + MUMPS (explicit and implicit)",
        "starter_linux64_gf": "Starter (GNU)",
        "engine_linux64_gf": "Engine, GNU, OpenMP only (explicit; no implicit)",
        "engine_linux64_gf_ompi": "Engine, GNU + OpenMPI 4 + MUMPS (explicit and implicit)",
        "engine_linux64_ifx_impi": "Engine, Intel + Intel MPI + MUMPS (explicit and implicit)",
    }
    lines += [f"  {e:<26} {desc.get(e, '')}" for e in exes]
    lines.append("")
    if osname == "win64":
        lines += [
            "Requirements: Windows 10/11 x64 with the Microsoft Visual C++ 2015-2022",
            "redistributable. The _impi Engine also needs the Intel MPI Library runtime",
            "(it provides mpiexec and impi.dll).",
            "",
            "Setup (cmd.exe):",
            "  set OPENRADIOSS_PATH=<folder containing exec>",
            "  set RAD_CFG_PATH=%OPENRADIOSS_PATH%\\hm_cfg_files",
            "  set RAD_H3D_PATH=%OPENRADIOSS_PATH%\\extlib\\h3d\\lib\\win64",
            "  set KMP_STACKSIZE=400m",
            "  set PATH=%OPENRADIOSS_PATH%\\extlib\\hm_reader\\win64;%OPENRADIOSS_PATH%\\extlib\\intelOneAPI_runtime\\win64;%PATH%",
            "  call \"<Intel MPI>\\env\\vars.bat\"     (for engine_win64_impi.exe)",
            "",
            "Run:",
            "  starter_win64.exe -i model_0000.rad -np N",
            "  engine_win64.exe -i model_0001.rad                  (explicit, OpenMP)",
            "  mpiexec -n N engine_win64_impi.exe -i model_0001.rad",
            "",
            "Self-test (implicit, MUMPS): qa-tests\\implicit\\cantilever\\run_cantilever.bat",
            "  (needs Python 3; prints PASS when the tip deflection matches 2.000 mm)",
        ]
    else:
        lines += [
            "Requirements (built on openSUSE Tumbleweed):",
            "  - GNU executables (_gf): glibc 2.44 or newer, and libstdc++ from GCC 13 or newer.",
            "  - engine_linux64_ifx_impi: glibc 2.38 or newer (e.g. Ubuntu 24.04), plus the",
            "    Intel MPI Library runtime (libmpi.so.12, mpiexec).",
            "  - engine_linux64_gf_ompi: OpenMPI 4.x (libmpi.so.40, mpiexec).",
            "",
            "Setup (bash):",
            "  export OPENRADIOSS_PATH=<folder containing exec>",
            "  export RAD_CFG_PATH=$OPENRADIOSS_PATH/hm_cfg_files",
            "  export RAD_H3D_PATH=$OPENRADIOSS_PATH/extlib/h3d/lib/linux64",
            "  export OMP_STACKSIZE=400m",
            "  export LD_LIBRARY_PATH=$OPENRADIOSS_PATH/extlib/hm_reader/linux64:$OPENRADIOSS_PATH/extlib/intelOneAPI_runtime/linux64:$LD_LIBRARY_PATH",
            "  OpenMPI 4 on PATH and LD_LIBRARY_PATH (for _gf_ompi), or",
            "  source <oneAPI>/mpi/latest/env/vars.sh (for _ifx_impi)",
            "",
            "Run:",
            "  starter_linux64_gf -i model_0000.rad -np N",
            "  engine_linux64_gf -i model_0001.rad                         (explicit, OpenMP)",
            "  mpiexec -n N engine_linux64_gf_ompi -i model_0001.rad",
            "  mpiexec -n N engine_linux64_ifx_impi -i model_0001.rad",
            "",
            "Self-test (implicit, MUMPS):",
            "  bash qa-tests/implicit/cantilever/run_cantilever.sh engine_linux64_gf_ompi",
            "  bash qa-tests/implicit/cantilever/run_cantilever.sh engine_linux64_ifx_impi",
            "  (needs Python 3; prints PASS when the tip deflection matches 2.000 mm)",
        ]
    lines += [
        "",
        "Implicit analysis: /IMPLICIT in the Starter deck; /IMPL/... cards in the Engine",
        "deck (see qa-tests/implicit/cantilever/README.md). It needs an MPI Engine; run",
        "on one rank with mpiexec -n 1 if needed. /IMPL/BUCKL and /EIG are not available.",
        "",
        "Limitation: this reader cannot generate /ALE/STRUCTURED_MESH; Starter rejects it.",
        "",
        "OpenRadioss is licensed under the GNU AGPL v3 (LICENSE.md). Third-party licenses",
        "are in licenses/.",
    ]
    return "\n".join(lines) + "\n"


def main():
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("--os", required=True, choices=sorted(EXECUTABLES))
    ap.add_argument("--out", default=str(ROOT / "bundles"), help="output folder (default: <repo>/bundles)")
    args = ap.parse_args()
    osname = args.os
    oneapi = oneapi_root(osname)

    exes = [e for e in EXECUTABLES[osname] if (ROOT / "exec" / e).exists()]
    missing = sorted(set(EXECUTABLES[osname]) - set(exes))
    if missing:
        print(f"warning: not built, left out: {', '.join(missing)}")
    if not any(e.startswith("starter") for e in exes) or not any(e.startswith("engine") for e in exes):
        sys.exit("error: need at least one Starter and one Engine in exec/")

    with tempfile.TemporaryDirectory() as tmp:
        stage = Path(tmp) / TOP
        for e in exes:
            copy(ROOT / "exec" / e, stage / "exec" / e)
        copy(ROOT / "extlib" / "h3d" / "lib" / osname, stage / "extlib" / "h3d" / "lib" / osname)
        copy(ROOT / "extlib" / "hm_reader" / osname, stage / "extlib" / "hm_reader" / osname)
        for f in (stage / "extlib" / "hm_reader" / osname).glob("*.lib"):
            f.unlink()  # import library, only needed to build
        if osname == "win64" or "engine_linux64_ifx_impi" in exes:
            dst = stage / "extlib" / "intelOneAPI_runtime" / osname
            for comp, sub, patterns in RUNTIME[osname]:
                src = oneapi / comp / "latest" / sub
                for pat in patterns:
                    found = glob.glob(str(src / pat))
                    if not found:
                        sys.exit(f"error: {pat} not found under {src}")
                    for f in found:
                        copy(Path(f), dst / Path(f).name)
        copy(ROOT / "hm_cfg_files", stage / "hm_cfg_files")
        test = ROOT / "qa-tests" / "implicit" / "cantilever"
        for f in test.iterdir():
            if f.is_file():
                copy(f, stage / "qa-tests" / "implicit" / "cantilever" / f.name)
        stage_licenses(osname, stage, oneapi)
        copy(ROOT / "COPYRIGHT.md", stage / "COPYRIGHT.md")
        copy(ROOT / "LICENSE.md", stage / "LICENSE.md")
        newline = "\r\n" if osname == "win64" else "\n"
        (stage / "README.txt").write_text(readme(osname, exes, oneapi), newline=newline)

        out = Path(args.out)
        out.mkdir(parents=True, exist_ok=True)
        archive = out / f"{TOP}_{osname}.zip"
        with zipfile.ZipFile(archive, "w", zipfile.ZIP_DEFLATED, compresslevel=6) as z:
            for p in sorted(stage.rglob("*")):
                arc = p.relative_to(stage.parent).as_posix()
                if p.is_dir():
                    continue
                info = zipfile.ZipInfo.from_file(p, arc)
                if osname != "win64":
                    # Keep Unix permissions, and make sure executables stay executable.
                    mode = p.stat().st_mode & 0o777
                    if p.parent.name == "exec" or p.suffix == ".sh":
                        mode |= 0o755
                    info.external_attr = (0o100000 | mode) << 16
                    info.create_system = 3
                info.compress_type = zipfile.ZIP_DEFLATED
                with open(p, "rb") as fh:
                    z.writestr(info, fh.read(), compresslevel=6)
    size = archive.stat().st_size / 2**20
    print(f"wrote {archive} ({size:.0f} MB): {', '.join(exes)}")


if __name__ == "__main__":
    main()
