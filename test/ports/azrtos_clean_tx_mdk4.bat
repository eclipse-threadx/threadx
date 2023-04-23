@ECHO OFF

ECHO INFO: Cleaning %CD%...

ECHO INFO: Cleaning ThreadX_Library...
DEL out.txt 2> NUL
REM ### bug in uv4? uv4 hungs on clean, uv5 works
REM START /wait %AZRTOS_KEIL_UV% -j0 -c ThreadX_Library.uvproj -o out.txt
IF %ERRORLEVEL% GEQ 2 GOTO CLEAN_ERROR
TYPE out.txt 2> NUL

ECHO INFO: Cleaning ThreadX_Demo...
DEL out.txt 2> NUL
REM ### bug in uv4? uv4 hungs on clean, uv5 works
REM START /wait %AZRTOS_KEIL_UV% -j0 -c ThreadX_Demo.uvproj -o out.txt
IF %ERRORLEVEL% GEQ 2 GOTO CLEAN_ERROR
TYPE out.txt 2> NUL

DEL out.txt 2> NUL
ECHO INFO: Cleaning completed successfully!
EXIT /B 0

:CLEAN_ERROR
TYPE out.txt 2> NUL
ECHO ERROR: clean failed.
EXIT /B 1
