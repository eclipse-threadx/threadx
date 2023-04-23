@ECHO OFF

ECHO INFO: Setting up the environment for ARM Developer Studio 2021.2...

SET "AZRTOS_TEST_DIR=%~dp0"

SET AZRTOS_ARM_DS_DIR="C:\Program Files\Arm\Development Studio 2021.2"
IF NOT EXIST %AZRTOS_ARM_DS_DIR% (
    ECHO ERROR: ARM Developer Studio directory not found
    EXIT /B 2
)

SET AZRTOS_ARM_DS_IDEC=%AZRTOS_ARM_DS_DIR%\bin\armds_idec.exe
IF NOT EXIST %AZRTOS_ARM_DS_IDEC% (
    ECHO ERROR: armds_idec.exe not found
    EXIT /B 2
) ELSE (
    ECHO INFO: armds_idec.exe found at %AZRTOS_ARM_DS_IDEC%
)

SET AZRTOS_ARM_DS_DBG=%AZRTOS_ARM_DS_DIR%\bin\armdbg.exe
IF NOT EXIST %AZRTOS_ARM_DS_DBG% (
    ECHO ERROR: armdbg.exe not found
    EXIT /B 2
) ELSE (
    ECHO INFO: armdbg.exe found at %AZRTOS_ARM_DS_DBG%
)

ECHO INFO: ARM Developer Studio 2021.2 environment setup completed.
