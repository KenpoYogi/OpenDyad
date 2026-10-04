#!/usr/bin/env bash
# Builds the output converters anim_to_vtk and th_to_csv into exec/ with gcc/g++.
set -euo pipefail

here=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
for tool in anim_to_vtk th_to_csv; do
    (cd "$here/$tool/linux64" && bash build.bash)
done
