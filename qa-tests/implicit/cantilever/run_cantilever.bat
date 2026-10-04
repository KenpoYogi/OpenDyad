@echo off
setlocal
rem Runs the implicit cantilever check: Starter, Engine (MPI), then check_cantilever.py.
rem Usage: run_cantilever.bat [engine] [ranks]
rem Uses OPENRADIOSS_PATH when set, otherwise this repo. Loads oneAPI when mpiexec is missing.
set "here=%~dp0"
set "engine=%~1"
if not defined engine set "engine=engine_win64_impi"
set "np=%~2"
if not defined np set "np=1"
if not defined OPENRADIOSS_PATH for %%I in ("%here%..\..\..") do set "OPENRADIOSS_PATH=%%~fI"
set "RAD_CFG_PATH=%OPENRADIOSS_PATH%\hm_cfg_files"
set "RAD_H3D_PATH=%OPENRADIOSS_PATH%\extlib\h3d\lib\win64"
set "PATH=%OPENRADIOSS_PATH%\extlib\hm_reader\win64;%PATH%"
if not defined KMP_STACKSIZE set "KMP_STACKSIZE=400m"
if not defined OMP_NUM_THREADS set "OMP_NUM_THREADS=1"

where mpiexec.exe >nul 2>&1
if errorlevel 1 call :load_oneapi
where mpiexec.exe >nul 2>&1
if not errorlevel 1 goto :run
echo mpiexec is not available. Install Intel MPI or load oneAPI first.
exit /b 2

:run
set "run=%TEMP%\openradioss_cantilever_%engine%_np%np%"
if exist "%run%" rmdir /s /q "%run%"
mkdir "%run%"
copy /y "%here%cantilever_0000.rad" "%run%" >nul
copy /y "%here%cantilever_0001.rad" "%run%" >nul
pushd "%run%"
"%OPENRADIOSS_PATH%\exec\starter_win64.exe" -i cantilever_0000.rad -np %np% > starter_console.txt 2>&1
set "starter_rc=%ERRORLEVEL%"
mpiexec -n %np% "%OPENRADIOSS_PATH%\exec\%engine%.exe" -i cantilever_0001.rad > engine_console.txt 2>&1
set "engine_rc=%ERRORLEVEL%"
popd
echo Run folder: %run%
if not "%starter_rc%"=="0" echo Starter exit code %starter_rc%, see starter_console.txt
if not "%engine_rc%"=="0" echo Engine exit code %engine_rc%, see engine_console.txt
python "%here%check_cantilever.py" "%run%"
exit /b %ERRORLEVEL%

:load_oneapi
rem setvars.bat calls vars.bat by bare name, which fails when this is set.
set NoDefaultCurrentDirectoryInExePath=
if not defined ONEAPI_ROOT set "ONEAPI_ROOT=%ProgramFiles(x86)%\Intel\oneAPI"
if not exist "%ONEAPI_ROOT%\setvars.bat" exit /b 0
call "%ONEAPI_ROOT%\setvars.bat" intel64 >nul 2>&1
exit /b 0
