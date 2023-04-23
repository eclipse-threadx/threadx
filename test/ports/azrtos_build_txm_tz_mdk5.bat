@ECHO OFF

ECHO INFO: Building %CD%...

ECHO INFO: Building txm...
PUSHD txm
DEL out.txt > NUL
%AZRTOS_KEIL_UV% -j -b txm.uvprojx -o out.txt
IF %ERRORLEVEL% GEQ 2 GOTO ERR_BUILD
TYPE out.txt 2> NUL
POPD

ECHO INFO: Building ThreadX_Library...
PUSHD .
DEL out.txt 2> NUL
%AZRTOS_KEIL_UV% -j -b ThreadX_Library.uvprojx -o out.txt
IF %ERRORLEVEL% GEQ 2 GOTO ERR_BUILD
type out.txt 2> NUL
POPD

ECHO INFO: Building demo_secure_zone...
PUSHD demo_secure_zone
DEL out.txt 2> NUL
%AZRTOS_KEIL_UV% -j -b demo_secure_zone.uvprojx -o out.txt
IF %ERRORLEVEL% GEQ 2 GOTO ERR_BUILD
type out.txt 2> NUL
POPD

ECHO INFO: Building demo_threadx_non-secure_zone...
PUSHD demo_threadx_non-secure_zone
DEL out.txt 2> NUL
%AZRTOS_KEIL_UV% -j -b demo_threadx_non-secure_zone.uvprojx -o out.txt
IF %ERRORLEVEL% GEQ 2 GOTO ERR_BUILD
type out.txt 2> NUL
POPD

ECHO INFO: Building sample_threadx_module...
PUSHD sample_threadx_module
DEL out.txt 2> NUL
%AZRTOS_KEIL_UV% -j -b sample_threadx_module.uvprojx -o out.txt
IF %ERRORLEVEL% GEQ 2 GOTO ERR_BUILD
type out.txt 2> NUL
POPD

DEL out.txt 2> NUL
ECHO INFO: Build completed successfully!
EXIT /B 0

:ERR_BUILD
TYPE out.txt 2> NUL
POPD
echo ERROR: build failed.
EXIT /B 1
