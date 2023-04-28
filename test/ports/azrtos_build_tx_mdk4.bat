@ECHO OFF

ECHO INFO: Building %CD%...

ECHO INFO: Building ThreadX_Library...
DEL out.txt 2> NUL
START /wait %AZRTOS_KEIL_UV% -j0 -b ThreadX_Library.uvproj -o out.txt
IF %ERRORLEVEL% GEQ 2 GOTO BUILD_ERROR
TYPE out.txt 2> NUL

ECHO INFO: Building ThreadX_Demo...
DEL out.txt 2> NUL
START /wait %AZRTOS_KEIL_UV% -j0 -b ThreadX_Demo.uvproj -o out.txt
IF %ERRORLEVEL% GEQ 2 GOTO BUILD_ERROR
TYPE out.txt 2> NUL

DEL out.txt 2> NUL
ECHO INFO: Build completed successfully!
EXIT /B 0

:BUILD_ERROR
TYPE out.txt 2> NUL
ECHO ERROR: build failed.
EXIT /B 1
