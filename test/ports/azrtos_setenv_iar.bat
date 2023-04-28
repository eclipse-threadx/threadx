@ECHO OFF

ECHO INFO: Setting up the environment for IAR...

IF EXIST %AZRTOS_IAR_DIR% GOTO FOUND_IAR_DIR
SET AZRTOS_IAR_DIR="C:\Program Files (x86)\IAR Systems\Embedded Workbench 8.4_2"
IF EXIST %AZRTOS_IAR_DIR% GOTO FOUND_IAR_DIR
SET AZRTOS_IAR_DIR="C:\Program Files (x86)\IAR Systems\Embedded Workbench 8.4"
IF EXIST %AZRTOS_IAR_DIR% GOTO FOUND_IAR_DIR
SET AZRTOS_IAR_DIR="C:\Program Files (x86)\IAR Systems\Embedded Workbench 8.3"
IF EXIST %AZRTOS_IAR_DIR% GOTO FOUND_IAR_DIR
SET AZRTOS_IAR_DIR="C:\Program Files (x86)\IAR Systems\Embedded Workbench 8.2"
IF EXIST %AZRTOS_IAR_DIR% GOTO FOUND_IAR_DIR
ECHO ERROR: IAR Embedded Workbench not found.
EXIT /B 2
:FOUND_IAR_DIR

SET AZRTOS_IAR_BUILD=%AZRTOS_IAR_DIR%\common\bin\IarBuild.exe
IF NOT EXIST %AZRTOS_IAR_BUILD% (
    ECHO ERROR: IarBuild.exe not found.
    EXIT /B 2
) ELSE (
    ECHO INFO: IarBuild.exe found at %AZRTOS_IAR_BUILD%
)

ECHO INFO: IAR environment setup completed successfully!
