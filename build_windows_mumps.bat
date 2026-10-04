@echo off
setlocal
rem Builds Starter and an Intel MPI Engine with MUMPS (implicit solver enabled).
rem Usage: build_windows_mumps.bat [jobs]
rem Needs MUMPS 5.5.1 extracted in engine\extlib\MUMPS_5.5.1.
rem Loads the oneAPI environment itself when ifx is not already on PATH.
set "source_root=%~dp0"
set "jobs=%~1"
if not defined jobs set "jobs=%NUMBER_OF_PROCESSORS%"
if not defined jobs set "jobs=4"

if not exist "%source_root%engine\extlib\MUMPS_5.5.1\src" (
    echo MUMPS 5.5.1 not found in engine\extlib\MUMPS_5.5.1
    exit /b 1
)

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

python "%source_root%Compiling_tools\script\load_extlib.py"
if errorlevel 1 exit /b %errorlevel%

cmake -S "%source_root%starter" -B "%source_root%starter\cbuild_win64" -G Ninja -DVS_BUILD=1 -Darch=win64 -Dprecision=dp -Ddebug=0 -Dstatic_link=0 -DEXEC_NAME=starter_win64 -DUSE_OPEN_READER=0 -DHM_READER_LEGACY_INCLUDE_API=ON -DHM_READER_LEGACY_PART_API=OFF -DHM_READER_NO_STRUCTURED_ALE=ON -DCMAKE_BUILD_TYPE=Release -DCMAKE_Fortran_COMPILER=ifx.exe -DCMAKE_C_COMPILER=icx.exe -DCMAKE_CXX_COMPILER=icx.exe
if errorlevel 1 exit /b %errorlevel%
cmake --build "%source_root%starter\cbuild_win64" --parallel "%jobs%"
if errorlevel 1 exit /b %errorlevel%

cmake -S "%source_root%engine" -B "%source_root%engine\cbuild_win64_impi" -G Ninja -DVS_BUILD=1 -Darch=win64 -Dprecision=dp -Ddebug=0 -Dstatic_link=0 -DMPI=impi -DEXEC_NAME=engine_win64_impi -DCMAKE_BUILD_TYPE=Release -DCMAKE_Fortran_COMPILER=ifx.exe -DCMAKE_C_COMPILER=icx.exe -DCMAKE_CXX_COMPILER=icx.exe
if errorlevel 1 exit /b %errorlevel%
cmake --build "%source_root%engine\cbuild_win64_impi" --parallel "%jobs%"
if errorlevel 1 exit /b %errorlevel%

copy /Y "%source_root%extlib\hm_reader\win64\hm_reader_win64.dll" "%source_root%exec\hm_reader_win64.dll" >nul
if errorlevel 1 exit /b %errorlevel%
echo Executables are in %source_root%exec
echo Run the Engine with: mpiexec -n 1 engine_win64_impi.exe -i [Engine input file]
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
