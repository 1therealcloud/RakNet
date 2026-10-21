@echo off
setlocal EnableExtensions

rem build raknet with visual c++ toolkit 2003 (msvc 7.1)
set "ROOT=%~dp0"
set "VC=C:\Program Files (x86)\Microsoft Visual C++ Toolkit 2003"
set "SDK=C:\Program Files\Microsoft Platform SDK"

if not exist "%VC%\bin\cl.exe" goto :missing_cl
if not exist "%VC%\bin\link.exe" goto :missing_link
if not exist "%SDK%\Include\windows.h" goto :missing_sdk
if not exist "%ROOT%Source\RakPeer.cpp" goto :missing_source

set "PATH=%VC%\bin;%SDK%\Bin;%PATH%"
set "INCLUDE=%VC%\include;%SDK%\Include;%INCLUDE%"
set "LIB=%VC%\lib;%SDK%\Lib;%LIB%"

set "BUILD=%ROOT%build"
set "OBJ=%BUILD%\obj\raknet"
set "OUT=%BUILD%\RakNet.lib"
set "RSP=%OBJ%\objects.rsp"

if not exist "%BUILD%" mkdir "%BUILD%"
if not exist "%BUILD%\obj" mkdir "%BUILD%\obj"
if not exist "%OBJ%" mkdir "%OBJ%"
if exist "%RSP%" del /Q "%RSP%"
if exist "%OUT%" del /Q "%OUT%"

echo Building RakNet with MSVC 7.1...
for %%F in ("%ROOT%Source\*.cpp") do (
    echo [CXX] %%~nxF
    "%VC%\bin\cl.exe" /nologo /c /O2 /MT /GX /I"%ROOT%Include" /Fo"%OBJ%\%%~nF.obj" "%%~fF"
    if errorlevel 1 goto :failed
    >>"%RSP%" echo "%OBJ%\%%~nF.obj"
)

if not exist "%RSP%" goto :failed

echo [LIB] RakNet.lib
"%VC%\bin\link.exe" /LIB /NOLOGO /MACHINE:X86 /OUT:"%OUT%" @"%RSP%"
if errorlevel 1 goto :failed

echo.
echo Built: "%OUT%"
exit /b 0

:missing_cl
echo ERROR: cl.exe not found in "%VC%\bin"
exit /b 1

:missing_link
echo ERROR: link.exe not found in "%VC%\bin"
exit /b 1

:missing_sdk
echo ERROR: Platform SDK headers not found in "%SDK%\Include"
exit /b 1

:missing_source
echo ERROR: run this script from the RakNet project root.
exit /b 1

:failed
echo ERROR: RakNet build failed.
exit /b 1
