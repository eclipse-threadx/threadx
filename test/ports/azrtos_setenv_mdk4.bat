@ECHO OFF

ECHO INFO: Setting up the environment for KEIL MDK v4...

SET AZRTOS_KEIL_DIR=C:\Keil\UV4
IF EXIST %AZRTOS_KEIL_DIR% GOTO FOUND_KEIL_UV_DIR
ECHO ERROR: KEIL uVision directory not found
EXIT /B 1

:FOUND_KEIL_UV_DIR
SET AZRTOS_KEIL_UV=%AZRTOS_KEIL_DIR%\uv4.exe
IF EXIST %AZRTOS_KEIL_UV% GOTO FOUND_KEIL_UV
ECHO ERROR: uv4.exe not found
EXIT /B 1

:FOUND_KEIL_UV
ECHO INFO: uv4.exe found at %AZRTOS_KEIL_UV%

ECHO INFO: KEIL uVision environment setup completed successfully!
