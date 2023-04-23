@ECHO OFF

ECHO INFO: Building %CD%...

ECHO INFO: Building ThreadX_Library...
PUSHD .
DEL out.txt 2> NUL
%AZRTOS_KEIL_UV% -j0 -b ThreadX_Library.uvprojx -o out.txt
IF %ERRORLEVEL% NEQ 0 GOTO ERR_BUILD
TYPE out.txt 2> NUL
POPD

ECHO INFO: Building demo_secure_zone...
PUSHD demo_secure_zone
DEL out.txt 2> NUL
%AZRTOS_KEIL_UV% -j0 -b demo_secure_zone.uvprojx -o out.txt
IF %ERRORLEVEL% NEQ 0 GOTO ERR_BUILD
TYPE out.txt 2> NUL
POPD

ECHO INFO: Building demo_threadx_non-secure_zone...
PUSHD demo_threadx_non-secure_zone
DEL out.txt 2> NUL
%AZRTOS_KEIL_UV% -j0 -b demo_threadx_non-secure_zone.uvprojx -o out.txt
IF %ERRORLEVEL% NEQ 0 GOTO ERR_BUILD
TYPE out.txt 2> NUL
POPD

REM ECHO Building ThreadX_Demo...
REM PUSHD demo_threadx_non-secure_zone
REM DEL out.txt 2> NUL
REM REM %AZRTOS_KEIL_UV% -j0 -b ThreadX_Demo.uvproj -o out.txt
REM IF %ERRORLEVEL% NEQ 0 GOTO ERR_BUILD
REM TYPE out.txt 2>NUL
REM POPD

DEL out.txt 2> NUL
ECHO INFO: Build completed successfully!
EXIT /B 0

:ERR_BUILD
TYPE out.txt 2> NUL
POPD
echo ERROR: build failed.
EXIT /B 1
