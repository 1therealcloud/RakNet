@echo off

REM  Remember, all of these commands are relative to the current directory
REM  Running this batch file from the wrong directory could remove 
REM	files and directories unintentionally

REM	If you didn't already know, you can turn on all output by removing the "REM" from  the line below
REM @echo on

REM	First, the simple case, remove some files with specific extensions
for %%a in (pdb idb ilk exp) do del /q /s /f "Lib\*.%%a

del /q /s /f "Source\*.o"

REM	Remove all of the listed directories anywhere they occur in the current tree
for /D /R %%a in (Debug Release CompletedJobs Uploads EventLogs JobLogs _sgbak) do if exist "%%a\*.*" rd /q /s "%%a"

REM Remove specific libs
for %%a in (RakVoiceLibStaticDebug RakVoiceDebug RakVoiceDLLDebug zLibStaticDebug) do del /f "lib\%%a.lib"

del "Samples\TestDLL\*.dll"
del "DistributedProcessor\DPClientConsole\*.log"
del /f "DistributedProcessor\DPClientConsole\*.ncb"
del /f "DistributedProcessor\DPClientConsole\*.dll"
del /f "DistributedProcessor\DPClientConsole\*.suo"
del /f "DistributedProcessor\DPNode\*.lib"
del "DistributedProcessor\DPServerConsole\*.job"
del "DistributedProcessor\DPServerConsole\*.ilk"

rd /q /s build
del /f /ah *.suo
del /f *.ncb
del /f /ah "DistributedProcessor\*.suo"
del /f "DistributedProcessor\*.ncb"
del /f Makefile.win
del /f RakNet.a
del /f *.depend
del /f *.layout
REM c:\converter\prjconverter.exe c:\RakNet\RakNet.sln

@echo Don't forget to update readme.txt, revision.log.html, and move or delete the old build.
PAUSE