@echo off
setlocal EnableExtensions

rem visual c++ toolkit 2003 (msvc 7.1)
set "ROOT=%~dp0"
set "VC=C:\Program Files (x86)\Microsoft Visual C++ Toolkit 2003"
set "SDK=C:\Program Files\Microsoft Platform SDK"

if not exist "%VC%\bin\cl.exe" goto :missing_cl
if not exist "%VC%\bin\link.exe" goto :missing_link
if not exist "%SDK%\Include\windows.h" goto :missing_sdk
if not exist "%ROOT%Source\RakPeer.cpp" goto :missing_source

set "PATH=%VC%\bin;%SDK%\Bin;%PATH%"
set "INCLUDE=%ROOT%Include;%VC%\include;%SDK%\Include;%INCLUDE%"
set "LIB=%VC%\lib;%SDK%\Lib;%LIB%"

set "BUILD=%ROOT%build"
set "OBJ=%BUILD%\obj\raknet"
set "SAMPLE_OBJ=%BUILD%\obj\samples"
set "BIN=%BUILD%\samples"
set "RAKNET=%BUILD%\RakNet.lib"
set "RSP=%OBJ%\objects.rsp"

if not exist "%OBJ%" mkdir "%OBJ%"
if not exist "%SAMPLE_OBJ%" mkdir "%SAMPLE_OBJ%"
if not exist "%BIN%" mkdir "%BIN%"
if exist "%RSP%" del /Q "%RSP%"
if exist "%RAKNET%" del /Q "%RAKNET%"

echo Building RakNet with MSVC 7.1...
for %%F in ("%ROOT%Source\*.cpp") do (
    "%VC%\bin\cl.exe" /nologo /c /O2 /MT /GX /I"%ROOT%Include" /Fo"%OBJ%\%%~nF.obj" "%%~fF"
    if errorlevel 1 goto :failed
    >>"%RSP%" echo "%OBJ%\%%~nF.obj"
)

if not exist "%RSP%" goto :failed

echo [LINK] RakNet.lib
"%VC%\bin\link.exe" /LIB /NOLOGO /MACHINE:X86 /OUT:"%RAKNET%" @"%RSP%"
if errorlevel 1 goto :failed

echo.
echo Building samples...
call :sample BigPacketTest "Samples\BigPacketTest\BigPacketTest.cpp" "" 1
if errorlevel 1 goto :failed
call :sample BitStream "Samples\BitStream\BitStreamSample.cpp" "" 1
if errorlevel 1 goto :failed
call :sample DroppedConnectionTest "Samples\Dropped Connection Test\DroppedConnectionTest.cpp" "" 1
if errorlevel 1 goto :failed
call :sample ReliableOrderedTest "Samples\Reliable Ordered Test\ReliableOrderedTest.cpp" "" 1
if errorlevel 1 goto :failed
call :sample RPC "Samples\Remote Procedure Calls\RPC.cpp" "" 1
if errorlevel 1 goto :failed
call :sample QueryServer "Samples\Query\server.cpp" "Samples\Query\Query.cpp" 1
if errorlevel 1 goto :failed
call :sample QueryClient "Samples\Query\client.cpp" "" 0
if errorlevel 1 goto :failed
call :sample RCONServer "Samples\RCON\server.cpp" "Samples\Query\Query.cpp" 1
if errorlevel 1 goto :failed
call :sample RCONClient "Samples\RCON\client.cpp" "" 0
if errorlevel 1 goto :failed

echo.
echo Built: "%RAKNET%"
echo Built all 9 samples in "%BIN%"
exit /b 0

:sample
set "NAME=%~1"
set "MAIN=%ROOT%%~2"
set "SUPPORT=%~3"
set "USE_RAKNET=%~4"
set "SAMPLE_RSP=%SAMPLE_OBJ%\%NAME%.rsp"

if not exist "%MAIN%" (
    echo ERROR: Missing "%MAIN%"
    exit /b 1
)

"%VC%\bin\cl.exe" /nologo /c /O2 /MT /GX /I"%ROOT%Include" /Fo"%SAMPLE_OBJ%\%NAME%.obj" "%MAIN%"
if errorlevel 1 exit /b 1
>"%SAMPLE_RSP%" echo "%SAMPLE_OBJ%\%NAME%.obj"

if not "%SUPPORT%"=="" (
    if not exist "%ROOT%%SUPPORT%" (
        echo ERROR: Missing "%ROOT%%SUPPORT%"
        exit /b 1
    )
    "%VC%\bin\cl.exe" /nologo /c /O2 /MT /GX /I"%ROOT%Include" /Fo"%SAMPLE_OBJ%\%NAME%_query.obj" "%ROOT%%SUPPORT%"
    if errorlevel 1 exit /b 1
    >>"%SAMPLE_RSP%" echo "%SAMPLE_OBJ%\%NAME%_query.obj"
)

if "%USE_RAKNET%"=="1" >>"%SAMPLE_RSP%" echo "%RAKNET%"

echo [LINK] %NAME%.exe
"%VC%\bin\link.exe" /NOLOGO /MACHINE:X86 /SUBSYSTEM:CONSOLE /OUT:"%BIN%\%NAME%.exe" @"%SAMPLE_RSP%" ws2_32.lib winmm.lib kernel32.lib user32.lib
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

:missing_source
echo ERROR: Run this script from the RakNet project root.
exit /b 1

:failed
echo ERROR: Build failed.
exit /b 1
