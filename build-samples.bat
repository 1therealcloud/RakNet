@echo off
setlocal EnableExtensions

rem build samples with visual c++ toolkit 2003 (msvc 7.1)
set "ROOT=%~dp0"
set "VC=C:\Program Files (x86)\Microsoft Visual C++ Toolkit 2003"
set "SDK=C:\Program Files\Microsoft Platform SDK"

if not exist "%VC%\bin\cl.exe" goto :missing_cl
if not exist "%VC%\bin\link.exe" goto :missing_link
if not exist "%SDK%\Include\windows.h" goto :missing_sdk
if not exist "%ROOT%build\RakNet.lib" goto :missing_raknet

set "PATH=%VC%\bin;%SDK%\Bin;%PATH%"
set "INCLUDE=%ROOT%Include;%VC%\include;%SDK%\Include;%INCLUDE%"
set "LIB=%VC%\lib;%SDK%\Lib;%LIB%"

set "OBJ=%ROOT%build\obj\samples"
set "BIN=%ROOT%build\samples"
set "RAKNET=%ROOT%build\RakNet.lib"

if not exist "%ROOT%build\obj" mkdir "%ROOT%build\obj"
if not exist "%OBJ%" mkdir "%OBJ%"
if not exist "%BIN%" mkdir "%BIN%"

echo Building RakNet samples with MSVC 7.1...
call :build_sample "BigPacketTest" "Samples\BigPacketTest\BigPacketTest.cpp" "" 1
if errorlevel 1 goto :failed
call :build_sample "BitStream" "Samples\BitStream\BitStreamSample.cpp" "" 1
if errorlevel 1 goto :failed
call :build_sample "DroppedConnectionTest" "Samples\Dropped Connection Test\DroppedConnectionTest.cpp" "" 1
if errorlevel 1 goto :failed
call :build_sample "ReliableOrderedTest" "Samples\Reliable Ordered Test\ReliableOrderedTest.cpp" "" 1
if errorlevel 1 goto :failed
call :build_sample "RPC" "Samples\Remote Procedure Calls\RPC.cpp" "" 1
if errorlevel 1 goto :failed
call :build_sample "QueryServer" "Samples\Query\server.cpp" "Samples\Query\Query.cpp" 1
if errorlevel 1 goto :failed
call :build_sample "QueryClient" "Samples\Query\client.cpp" "" 0
if errorlevel 1 goto :failed
call :build_sample "RCONServer" "Samples\RCON\server.cpp" "Samples\Query\Query.cpp" 1
if errorlevel 1 goto :failed
call :build_sample "RCONClient" "Samples\RCON\client.cpp" "" 0
if errorlevel 1 goto :failed

echo.
echo Built all 9 samples in "%BIN%"
exit /b 0

:build_sample
set "NAME=%~1"
set "MAIN=%ROOT%%~2"
set "SUPPORT=%~3"
set "USE_RAKNET=%~4"
set "RSP=%OBJ%\%NAME%.rsp"

if not exist "%MAIN%" (
    echo ERROR: Missing "%MAIN%"
    exit /b 1
)

echo [CXX] %NAME%
"%VC%\bin\cl.exe" /nologo /c /O2 /MT /GX /I"%ROOT%Include" /Fo"%OBJ%\%NAME%.obj" "%MAIN%"
if errorlevel 1 exit /b 1
>"%RSP%" echo "%OBJ%\%NAME%.obj"

if not "%SUPPORT%"=="" (
    if not exist "%ROOT%%SUPPORT%" (
        echo ERROR: Missing "%ROOT%%SUPPORT%"
        exit /b 1
    )
    "%VC%\bin\cl.exe" /nologo /c /O2 /MT /GX /I"%ROOT%Include" /Fo"%OBJ%\%NAME%_query.obj" "%ROOT%%SUPPORT%"
    if errorlevel 1 exit /b 1
    >>"%RSP%" echo "%OBJ%\%NAME%_query.obj"
)

if "%USE_RAKNET%"=="1" >>"%RSP%" echo "%RAKNET%"

echo [LINK] %NAME%.exe
"%VC%\bin\link.exe" /NOLOGO /MACHINE:X86 /SUBSYSTEM:CONSOLE /OUT:"%BIN%\%NAME%.exe" @"%RSP%" ws2_32.lib winmm.lib kernel32.lib user32.lib
if errorlevel 1 exit /b 1
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

:missing_raknet
echo ERROR: build\RakNet.lib is missing. Run build_raknet.bat first.
exit /b 1

:failed
echo ERROR: Samples build failed.
exit /b 1
