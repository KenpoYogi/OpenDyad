# OpenRadioss

For the community continuation of OpenRadioss, visit
[OpenCourant](https://github.com/OpenCourant/OpenCourant).

## What is OpenRadioss?

**OpenRadioss** is an open-source finite element solver for highly nonlinear
problems under dynamic loading. It provides the computational foundation for
studying crashworthiness, impact, structural deformation, contact, material
failure and fluid–structure interaction.

Its finite element and particle formulations support research on metals,
composites, polymers, concrete and other engineering materials. Applications
include vehicle safety, lightweight structures, manufacturing, protective
systems and coupled fluid–structure simulations.

This repository preserves the upstream source snapshot dated **2026-09-29**,
with Windows and Linux build dependencies from OpenCourant's `v82-hybrid` package.

## Getting started

- [Quick start](doc/Getting_started.md)
- [Build and run this repository](doc/BUILDING.md)
- [Compiler and platform guide](HOWTO.md)
- [Solver execution guide](INSTALL.md)
- [Download source ZIP](https://github.com/lililii124/openradioss-261001/archive/refs/heads/main.zip)

### Build on Linux or WSL

The bundled Linux reader supports a compatibility build. It cannot generate
`/ALE/STRUCTURED_MESH`; Starter reports an error for that keyword. Ordinary ALE
models with an existing mesh use the normal solver path.

Install GCC/GFortran, CMake, Make and Python 3, then run:

```bash
git clone https://github.com/lililii124/openradioss-261001.git
cd openradioss-261001
bash build_linux.sh 8
```

The build produces double-precision Starter and SMP Engine executables in
`exec/`. The external libraries and input reader are included in `extlib/`.

### Build on Windows

Install Intel oneAPI with MKL, Visual Studio C++ Build Tools and a Windows SDK,
plus CMake, Ninja and Python 3. Extract the source to a short path without spaces,
then run from a oneAPI command prompt:

```bat
build_windows_compat.bat 8
```

This compatibility build also rejects `/ALE/STRUCTURED_MESH`. Use an explicitly
defined mesh for ALE analyses.

Runtime paths, compiler details, structured ALE limitations and the
include-list compatibility behavior are documented in
[Building and running](doc/BUILDING.md).

## Solver workflow

**Starter** reads the input model, checks its definition and writes the restart
files required by **Engine**. Engine advances the simulation and writes the
requested time histories and field results.

The repository includes [example and regression models](qa-tests/miniqa)
covering elements, material laws, contacts, imposed loading, fluids and particle
methods. See [build and run instructions](doc/BUILDING.md) for the environment
settings and a small example.

## Input formats

- **`.rad`** — native Radioss input decks.
- **`.k` / `.key`** — keyword decks supported by the included input converter.

Available keywords and conversion behavior are described in the solver
reference documentation. A model should be checked in Starter before running
Engine.

## Results and visualization

Native animation and time-history output can be converted for visualization
and analysis:

- [Animation to VTK](tools/anim_to_vtk) for use with ParaView.
- [Time history to CSV](tools/th_to_csv) for plotting and numerical analysis.
- H3D output for compatible viewers, using the included H3D writer libraries.

Gmsh can be used for mesh generation and preprocessing. The
[preprocessing and visualization guide](https://openradioss.atlassian.net/wiki/spaces/OPENRADIOSS/pages/21397510/Pre%20and%20Post%20Processing%20for%20OpenRadioss)
describes common workflows.

## Contributing

Use this repository's [issues](https://github.com/lililii124/openradioss-261001/issues)
and [pull requests](https://github.com/lililii124/openradioss-261001/pulls) for
build problems, reproducible solver issues and proposed changes.

- [Coding and contribution guidance](CONTRIBUTING.md)
- [Code of conduct](CODE_OF_CONDUCT.md)
- [Internal solver documentation](doc)

## Documentation and references

- [Radioss reference documentation](https://help.altair.com/hwsolvers/rad/index.htm)
- [Reference guide, PDF](https://2022.help.altair.com/2022/simulation/pdfs/radopen/AltairRadioss_2022_ReferenceGuide.pdf)
- [User guide, PDF](https://2022.help.altair.com/2022/simulation/pdfs/radopen/AltairRadioss_2022_UserGuide.pdf)
- [Theory manual, PDF](https://2022.help.altair.com/2022/simulation/pdfs/radopen/AltairRadioss_2022_TheoryManual.pdf)
- [Original project documentation](https://openradioss.atlassian.net/wiki/spaces/OPENRADIOSS/pages/1016047/OpenRadioss+Documentation)

## License

[GNU AGPLv3](LICENSE.md). Original [copyright notices](COPYRIGHT.md) and
third-party notices are preserved with their corresponding files.
