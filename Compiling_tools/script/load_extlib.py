"""Check the external libraries shipped with this source tree."""

import json
from pathlib import Path
import sys


def main():
    source_root = Path(__file__).resolve().parents[2]
    with (source_root / "EXTLIB_VERSION.json").open(encoding="utf-8") as stream:
        manifest = json.load(stream)

    extlib = source_root / "extlib"
    missing = [name for name in manifest["required_files"] if not (extlib / name).is_file()]
    if missing:
        print("Missing bundled build dependencies:", file=sys.stderr)
        for name in missing:
            print(f"  extlib/{name}", file=sys.stderr)
        print("Restore the extlib directory from this repository or its source package.",
              file=sys.stderr)
        return 1

    print(f"Using bundled external libraries ({manifest['package_tag']}).")
    return 0


if __name__ == "__main__":
    sys.exit(main())
