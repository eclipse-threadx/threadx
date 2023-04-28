@ECHO OFF

ECHO INFO: Cleaning %CD%...

ECHO INFO: Cleaning ThreadX_Library...
DEL out.txt 2> NUL
START /wait %AZRTOS_KEIL_UV% -j0 -c ThreadX_Library.uvprojx -o out.txt
IF %ERRORLEVEL% GEQ 2 GOTO CLEAN_ERROR
TYPE out.txt 2> NUL

ECHO INFO: Cleaning ThreadX_Demo...
DEL out.txt 2> NUL
START /wait %AZRTOS_KEIL_UV% -j0 -c ThreadX_Demo.uvprojx -o out.txt
IF %ERRORLEVEL% GEQ 2 GOTO CLEAN_ERROR
TYPE out.txt 2> NUL

DEL out.txt 2> NUL
ECHO INFO: Cleaning completed successfully!
EXIT /B 0

:CLEAN_ERROR
TYPE out.txt 2> NUL
ECHO ERROR: clean failed.
EXIT /B 1
