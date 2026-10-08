#!/usr/bin/env python3
"""Stage the OpenRadioss GUI so it runs from a source build.

The GUI (tools/openradioss_gui) expects the release-bundle layout: exec/ and
hm_cfg_files/ one level above its own folder, and inp2rad.py beside it. This
copies it to <repo>/openradioss_gui (ignored by Git), puts inp2rad.py next to
it, byte-compiles everything as a syntax check, and reports whether tkinter is
available. It is the GUI step of build_windows_all.bat and build_linux_all.sh,
and can be run on its own:

    python tools/stage_gui.py
"""
import compileall
import os
import shutil
import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SRC = ROOT / "tools" / "openradioss_gui"
INP2RAD = ROOT / "tools" / "inp2rad" / "inp2rad" / "inp2rad.py"
DST = ROOT / "openradioss_gui"


def main():
    if not (SRC / "OpenRadioss_gui.py").exists():
        sys.exit(f"error: {SRC} is missing OpenRadioss_gui.py")
    if not INP2RAD.exists():
        sys.exit(f"error: {INP2RAD} not found")

    if DST.exists():
        shutil.rmtree(DST)
    shutil.copytree(SRC, DST, ignore=shutil.ignore_patterns("__pycache__", "*.pyc"))
    shutil.copy2(INP2RAD, DST / "inp2rad.py")
    if os.name != "nt":
        for f in DST.glob("*.bash"):
            f.chmod(f.stat().st_mode | 0o755)

    # Syntax check. Keep the .pyc files out of the staged folder, like the launchers do.
    with tempfile.TemporaryDirectory() as cache:
        ok = _compile(DST, cache)
    if not ok:
        sys.exit("error: the GUI sources did not compile")

    try:
        import tkinter  # noqa: F401
        tk = "available"
    except ImportError:
        tk = ("NOT available: install python3-tk (Debian/Ubuntu), python3xx-tk (openSUSE), "
              "or tick tcl/tk in the python.org installer")

    rel = DST.relative_to(ROOT)
    print(f"GUI staged in {rel}{os.sep} with inp2rad.py (Python {sys.version.split()[0]}, tkinter {tk})")
    if os.name == "nt":
        print(f"Start it with {rel}\\OpenRadioss_gui.vbs or {rel}\\OpenRadioss_gui.bat")
        print(r"Set Config > MPI Path to the Intel MPI folder, e.g. C:\Program Files (x86)\Intel\oneAPI\mpi\latest")
    else:
        print(f"Start it with: bash {rel}/OpenRadioss_gui.bash")
        print("Set Config > MPI Path to the OpenMPI folder, e.g. /usr/lib64/mpi/gcc/openmpi4")


def _compile(folder, cache):
    old = os.environ.get("PYTHONPYCACHEPREFIX")
    os.environ["PYTHONPYCACHEPREFIX"] = cache
    sys.pycache_prefix = cache
    try:
        return compileall.compile_dir(str(folder), quiet=1, force=True)
    finally:
        sys.pycache_prefix = None
        if old is None:
            os.environ.pop("PYTHONPYCACHEPREFIX", None)
        else:
            os.environ["PYTHONPYCACHEPREFIX"] = old


if __name__ == "__main__":
    main()
