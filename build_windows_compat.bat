@echo off
setlocal
rem Run from an Intel oneAPI command prompt with the compiler and MKL enabled.
set "source_root=%~dp0"
set "jobs=%~1"
if not defined jobs set "jobs=%NUMBER_OF_PROCESSORS%"
if not defined jobs set "jobs=4"

where ifx.exe >nul 2>&1
if errorlevel 1 (
    echo Intel Fortran is not available. Run this script from a oneAPI command prompt.
    exit /b 1
)
where icx.exe >nul 2>&1
if errorlevel 1 exit /b 1

python "%source_root%Compiling_tools\script\load_extlib.py"
if errorlevel 1 exit /b %errorlevel%

cmake -S "%source_root%starter" -B "%source_root%starter\cbuild_win64" -G Ninja -DVS_BUILD=1 -Darch=win64 -Dprecision=dp -Ddebug=0 -Dstatic_link=0 -DEXEC_NAME=starter_win64 -DUSE_OPEN_READER=0 -DHM_READER_LEGACY_INCLUDE_API=ON -DHM_READER_LEGACY_PART_API=OFF -DHM_READER_NO_STRUCTURED_ALE=ON -DCMAKE_BUILD_TYPE=Release -DCMAKE_Fortran_COMPILER=ifx.exe -DCMAKE_C_COMPILER=icx.exe -DCMAKE_CXX_COMPILER=icx.exe
if errorlevel 1 exit /b %errorlevel%
cmake --build "%source_root%starter\cbuild_win64" --parallel "%jobs%"
if errorlevel 1 exit /b %errorlevel%

cmake -S "%source_root%engine" -B "%source_root%engine\cbuild_win64" -G Ninja -DVS_BUILD=1 -Darch=win64 -Dprecision=dp -Ddebug=0 -Dstatic_link=0 -DMPI=smp -DEXEC_NAME=engine_win64 -DCMAKE_BUILD_TYPE=Release -DCMAKE_Fortran_COMPILER=ifx.exe -DCMAKE_C_COMPILER=icx.exe -DCMAKE_CXX_COMPILER=icx.exe
if errorlevel 1 exit /b %errorlevel%
cmake --build "%source_root%engine\cbuild_win64" --parallel "%jobs%"
if errorlevel 1 exit /b %errorlevel%

copy /Y "%source_root%extlib\hm_reader\win64\hm_reader_win64.dll" "%source_root%exec\hm_reader_win64.dll" >nul
if errorlevel 1 exit /b %errorlevel%
echo Executables are in %source_root%exec
echo Reader limitation: /ALE/STRUCTURED_MESH is not supported by this compatibility build.
