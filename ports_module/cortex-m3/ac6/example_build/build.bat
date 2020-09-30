@ECHO OFF

ECHO Build starting...

SETLOCAL ENABLEEXTENSIONS

IF DEFINED ARMDSIDEC GOTO IARBUILD_DEFINED
ECHO ERROR: please set ARMDSIDEC to the path of the ARM Developer Studio eclipsec.exe program
EXIT /B 2
:IARBUILD_DEFINED

IF EXIST %ARMDSIDEC% GOTO ARMDSIDEC_FOUND
ECHO ERROR: the command ARMDSIDEC doesn't exist: %ARMDSIDEC%
EXIT /B 2 
:ARMDSIDEC_FOUND

%ARMDSIDEC% -nosplash -application org.eclipse.cdt.managedbuilder.core.headlessbuild -data .\workspace -build all
IF %ERRORLEVEL% EQU 0 GOTO BUILD_OK
ECHO ERROR: build failed.
EXIT /B 1
:BUILD_OK

ECHO Build completed without errors.
EXIT /B 0
