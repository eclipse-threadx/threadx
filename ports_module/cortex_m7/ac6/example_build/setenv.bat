@ECHO OFF

SET ARMDSDIR="C:\Program Files\Arm\Development Studio 2020.0"
IF EXIST %ARMDSDIR% GOTO FOUND_ARMDS
ECHO ARM Development Studio not found.
EXIT /B 1

:FOUND_ARMDS
SET ARMDSIDEC=%ARMDSDIR%\bin\armds_idec.exe
IF EXIST %ARMDSIDEC% GOTO FOUND_ARMDS_IDEC
ECHO armds_idec.exe not found.
EXIT /B 1

:FOUND_ARMDS_IDEC
ECHO armds_idec.exe found at %ARMDSIDEC%
EXIT /B 0
