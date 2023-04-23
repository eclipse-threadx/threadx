@ECHO OFF

ECHO INFO: Building %CD%...

%AZRTOS_IAR_BUILD% tx.ewp -make Debug -log all -parallel %NUMBER_OF_PROCESSORS%
IF %ERRORLEVEL% NEQ 0 (
    ECHO ERROR: failed to build tx.ewp
    EXIT /B 1
)

%AZRTOS_IAR_BUILD% sample_threadx.ewp -make Debug -log all -parallel %NUMBER_OF_PROCESSORS%
IF %ERRORLEVEL% NEQ 0 (
    ECHO ERROR: failed to build sample_threadx.ewp
    EXIT /B 1
)

ECHO INFO: Build completed successfully!
