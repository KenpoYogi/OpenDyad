@echo off
setlocal
rem Default full build. One command builds everything the release bundle ships:
rem   1. starter_win64.exe, engine_win64_impi.exe
rem        double precision, Intel MPI + MUMPS: explicit and implicit  (build_windows_mumps.bat)
rem   2. starter_win64_sp.exe, engine_win64_sp.exe, engine_win64_impi_sp.exe
rem        single precision, explicit only                             (build_windows_sp.bat)
rem   3. anim_to_vtk_win64.exe, th_to_csv_win64.exe
rem        output converters                                            (tools\build_output_converters.bat)
rem   4. openradioss_gui\ at the repo root
rem        the OpenRadioss GUI with inp2rad.py, staged to run from here  (tools\stage_gui.py)
rem Downloads MUMPS 5.5.1 into engine\extlib when it is missing (SHA-256 checked).
rem Loads the oneAPI environment itself when ifx is not already on PATH.
rem
rem Usage: build_windows_all.bat [jobs] [-clean] [-smp] [-bundle]
rem   jobs     parallel build jobs (default: all processors)
rem   -clean   delete the Starter and Engine build directories (cbuild_win64*) first.
rem            Needed after moving or renaming the source tree: CMake refuses a stale cache.
rem   -smp     also build the OpenMP-only double-precision Engine engine_win64.exe
rem            (explicit only; the MPI Engine already runs explicit models on one rank)
rem   -bundle  package exec\ into bundles\OpenRadioss_win64.zip afterwards
set "source_root=%~dp0"
set "jobs="
set "clean=0"
set "smp=0"
set "bundle=0"

:parse_args
if "%~1"=="" goto :end_args
if /i "%~1"=="-clean" (set "clean=1" & shift & goto :parse_args)
if /i "%~1"=="-smp" (set "smp=1" & shift & goto :parse_args)
if /i "%~1"=="-bundle" (set "bundle=1" & shift & goto :parse_args)
if /i "%~1"=="-h" goto :usage
if /i "%~1"=="--help" goto :usage
if /i "%~1"=="/?" goto :usage
echo %~1| findstr /r "^[1-9][0-9]*$" >nul
if errorlevel 1 goto :usage
set "jobs=%~1"
shift
goto :parse_args
:end_args
if not defined jobs set "jobs=%NUMBER_OF_PROCESSORS%"
if not defined jobs set "jobs=4"

echo.
echo OpenDyad full build: double precision Intel MPI + MUMPS, single precision, output converters, GUI
if %smp%==1 echo   plus the OpenMP-only double-precision Engine
if %bundle%==1 echo   plus the release bundle
if %clean%==1 echo   from clean build directories
echo   %jobs% parallel jobs
echo.

if %clean%==0 goto :skip_clean
for %%d in (starter\cbuild_win64 starter\cbuild_win64_sp engine\cbuild_win64 engine\cbuild_win64_impi engine\cbuild_win64_sp engine\cbuild_win64_impi_sp) do (
    if exist "%source_root%%%d" (echo Removing %%d& rmdir /s /q "%source_root%%%d")
)
:skip_clean

rem --- Toolchain ----------------------------------------------------------
where ifx.exe >nul 2>&1
if errorlevel 1 call :load_oneapi
if errorlevel 1 exit /b 1
where ifx.exe >nul 2>&1
if errorlevel 1 (
    echo Intel Fortran is not available after loading oneAPI.
    exit /b 1
)
where link.exe >nul 2>&1
if errorlevel 1 (
    echo MSVC link.exe is not available. Set VS2026INSTALLDIR or VS2022INSTALLDIR.
    exit /b 1
)
if not defined I_MPI_ROOT (
    echo Intel MPI is not available. I_MPI_ROOT is not set.
    exit /b 1
)
where python.exe >nul 2>&1
if errorlevel 1 (
    echo Python 3 is not on PATH.
    exit /b 1
)

rem --- MUMPS source -------------------------------------------------------
if exist "%source_root%engine\extlib\MUMPS_5.5.1\src" goto :have_mumps
call :get_mumps
if errorlevel 1 exit /b 1
:have_mumps

rem --- 1. Double precision: Starter + Intel MPI Engine with MUMPS ---------
echo.
echo === [1/4] Double precision: starter_win64.exe, engine_win64_impi.exe ^(MUMPS^)
call "%source_root%build_windows_mumps.bat" %jobs%
if errorlevel 1 goto :failed

rem --- 2. Single precision: Starter + SMP and MPI Engines -----------------
echo.
echo === [2/4] Single precision: starter_win64_sp.exe, engine_win64_sp.exe, engine_win64_impi_sp.exe
call "%source_root%build_windows_sp.bat" %jobs%
if errorlevel 1 goto :failed

rem --- 3. Output converters ------------------------------------------------
echo.
echo === [3/4] Output converters: anim_to_vtk_win64.exe, th_to_csv_win64.exe
call "%source_root%tools\build_output_converters.bat"
if errorlevel 1 goto :failed

rem --- 4. OpenRadioss GUI --------------------------------------------------
echo.
echo === [4/4] OpenRadioss GUI: openradioss_gui\ ^(with inp2rad.py^)
python "%source_root%tools\stage_gui.py"
if errorlevel 1 goto :failed

rem --- Optional: OpenMP-only double-precision Engine ----------------------
if %smp%==0 goto :skip_smp
echo.
echo === [-smp] OpenMP-only double precision: engine_win64.exe
call "%source_root%build_windows_compat.bat" %jobs%
if errorlevel 1 goto :failed
:skip_smp

rem --- Optional: release bundle --------------------------------------------
if %bundle%==0 goto :skip_bundle
echo.
echo === [-bundle] Packaging bundles\OpenRadioss_win64.zip
python "%source_root%Compiling_tools\script\make_bundle.py" --os win64
if errorlevel 1 goto :failed
:skip_bundle

echo.
echo === Done. Executables in %source_root%exec:
dir /b "%source_root%exec\*.exe"
echo.
echo Self-test (implicit, MUMPS): qa-tests\implicit\cantilever\run_cantilever.bat
echo GUI: openradioss_gui\OpenRadioss_gui.vbs
exit /b 0

:failed
echo.
echo === Build FAILED ^(see the messages above^).
exit /b 1

:usage
echo Usage: build_windows_all.bat [jobs] [-clean] [-smp] [-bundle]
echo   jobs     number of parallel build jobs ^(default: all processors^)
echo   -clean   delete the Starter and Engine build directories first ^(after moving the source tree^)
echo   -smp     also build the OpenMP-only double-precision Engine engine_win64.exe
echo   -bundle  package exec\ into bundles\OpenRadioss_win64.zip afterwards
exit /b 1

:get_mumps
rem Same download as HOWTO_IMPLICIT.md, "Get the MUMPS source". curl, certutil and tar ship with Windows 10/11.
set "mumps_url=https://ftp.mcs.anl.gov/pub/petsc/externalpackages/MUMPS_5.5.1.tar.gz"
set "mumps_sha=1abff294fa47ee4cfd50dfd5c595942b72ebfcedce08142a75a99ab35014fa15"
set "mumps_tgz=%source_root%engine\extlib\MUMPS_5.5.1.tar.gz"
if exist "%mumps_tgz%" goto :check_mumps
echo Downloading MUMPS 5.5.1 to engine\extlib ...
curl -fL --retry 3 -o "%mumps_tgz%" "%mumps_url%"
if not errorlevel 1 goto :check_mumps
if exist "%mumps_tgz%" del "%mumps_tgz%"
echo Download failed. Fetch it yourself, see HOWTO_IMPLICIT.md, "Get the MUMPS source".
exit /b 1
:check_mumps
set "mumps_hash="
for /f "delims=" %%h in ('certutil -hashfile "%mumps_tgz%" SHA256 ^| findstr /r "^[0-9a-fA-F][0-9a-fA-F]*$"') do set "mumps_hash=%%h"
if /i "%mumps_hash%"=="%mumps_sha%" goto :extract_mumps
echo MUMPS_5.5.1.tar.gz checksum mismatch:
echo   got      %mumps_hash%
echo   expected %mumps_sha%
echo Delete engine\extlib\MUMPS_5.5.1.tar.gz and run again.
exit /b 1
:extract_mumps
echo Extracting MUMPS 5.5.1 ...
if exist "%source_root%engine\extlib\MUMPS_5.5.1" rmdir /s /q "%source_root%engine\extlib\MUMPS_5.5.1"
tar -xzf "%mumps_tgz%" -C "%source_root%engine\extlib"
if errorlevel 1 exit /b 1
if not exist "%source_root%engine\extlib\MUMPS_5.5.1\src" (
    echo MUMPS_5.5.1\src is missing after extraction.
    exit /b 1
)
del "%mumps_tgz%"
exit /b 0

:load_oneapi
rem setvars.bat only probes Community/Professional/Enterprise; point it at Build Tools.
if not defined VS2026INSTALLDIR if exist "%ProgramFiles(x86)%\Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" set "VS2026INSTALLDIR=%ProgramFiles(x86)%\Microsoft Visual Studio\18\BuildTools"
if not defined VS2022INSTALLDIR if exist "%ProgramFiles(x86)%\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" set "VS2022INSTALLDIR=%ProgramFiles(x86)%\Microsoft Visual Studio\2022\BuildTools"
rem setvars.bat calls vars.bat by bare name, which fails when this is set.
set NoDefaultCurrentDirectoryInExePath=
if not defined ONEAPI_ROOT set "ONEAPI_ROOT=%ProgramFiles(x86)%\Intel\oneAPI"
if exist "%ONEAPI_ROOT%\setvars.bat" goto :call_setvars
rem Kept outside a ( ) block: the path contains "(x86)".
echo oneAPI setvars.bat not found in %ONEAPI_ROOT%
exit /b 1
:call_setvars
call "%ONEAPI_ROOT%\setvars.bat" intel64
exit /b 0
