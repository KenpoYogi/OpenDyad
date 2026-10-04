@echo off
setlocal
rem Builds the output converters anim_to_vtk and th_to_csv into exec\ with MSVC.
rem Loads the Visual Studio x64 environment itself when cl.exe is not on PATH.
set "here=%~dp0"

where cl.exe >nul 2>&1
if errorlevel 1 call :load_msvc
where cl.exe >nul 2>&1
if not errorlevel 1 goto :build
echo MSVC cl.exe not found. Install the Visual Studio C++ Build Tools.
exit /b 1

:build
call :build_one anim_to_vtk || exit /b 1
call :build_one th_to_csv || exit /b 1
exit /b 0

:build_one
pushd "%here%%~1\win64"
call "%here%%~1\win64\build.bat"
set "rc=%ERRORLEVEL%"
popd
exit /b %rc%

:load_msvc
set "vswhere=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%vswhere%" exit /b 0
set "vsdir="
for /f "usebackq delims=" %%i in (`call "%vswhere%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "vsdir=%%i"
if not defined vsdir exit /b 0
call "%vsdir%\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
exit /b 0
