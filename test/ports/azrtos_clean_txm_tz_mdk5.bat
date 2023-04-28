@ECHO OFF

ECHO INFO: Cleaning %CD%...

ECHO INFO: Cleaning ThreadX_Library...
DEL out.txt 2> NUL
START /wait %AZRTOS_KEIL_UV% -j0 -c ThreadX_Library.uvprojx -o out.txt
IF %ERRORLEVEL% GEQ 2 GOTO CLEAN_ERROR
TYPE out.txt 2> NUL

ECHO INFO: Cleaning txm...
DEL out.txt 2> NUL
START /wait %AZRTOS_KEIL_UV% -j0 -c txm\txm.uvprojx -o out.txt
IF %ERRORLEVEL% GEQ 2 GOTO CLEAN_ERROR
TYPE out.txt 2> NUL

ECHO INFO: Cleaning demo_secure_zone...
DEL out.txt 2> NUL
START /wait %AZRTOS_KEIL_UV% -j0 -c demo_secure_zone\demo_secure_zone.uvprojx -o out.txt
IF %ERRORLEVEL% GEQ 2 GOTO CLEAN_ERROR
TYPE out.txt 2> NUL

ECHO INFO: Cleaning demo_threadx_non-secure_zone...
DEL out.txt 2> NUL
START /wait %AZRTOS_KEIL_UV% -j0 -c demo_threadx_non-secure_zone\demo_threadx_non-secure_zone.uvprojx -o out.txt
IF %ERRORLEVEL% GEQ 2 GOTO CLEAN_ERROR
TYPE out.txt 2> NUL

ECHO INFO: Cleaning sample_threadx_module...
DEL out.txt 2> NUL
START /wait %AZRTOS_KEIL_UV% -j0 -c sample_threadx_module\sample_threadx_module.uvprojx -o out.txt
IF %ERRORLEVEL% GEQ 2 GOTO CLEAN_ERROR
TYPE out.txt 2> NUL

DEL out.txt 2> NUL
ECHO INFO: Cleaning completed successfully!
EXIT /B 0

:CLEAN_ERROR
DEL out.txt 2> NUL
ECHO ERROR: clean failed.
EXIT /B 1
