@ECHO OFF

ECHO INFO: Cleaning %CD%...

%AZRTOS_IAR_BUILD% tx.ewp -clean Debug -log all
IF %ERRORLEVEL% NEQ 0 (
    ECHO ERROR: clean failed.
    EXIT /B 1
)

%AZRTOS_IAR_BUILD% sample_threadx.ewp -clean Debug -log all
IF %ERRORLEVEL% NEQ 0 (
    ECHO ERROR: clean failed.
    EXIT /B 1
)

%AZRTOS_IAR_BUILD% txm.ewp -clean Debug -log all
IF %ERRORLEVEL% NEQ 0 (
    ECHO ERROR: clean failed.
    EXIT /B 1
)

%AZRTOS_IAR_BUILD% sample_threadx_module.ewp -clean Debug -log all
IF %ERRORLEVEL% NEQ 0 (
    ECHO ERROR: clean failed.
    EXIT /B 1
)

%AZRTOS_IAR_BUILD% sample_threadx_module_manager.ewp -clean Debug -log all
IF %ERRORLEVEL% NEQ 0 (
    ECHO ERROR: clean failed.
    EXIT /B 1
)

RMDIR /Q /S Debug

ECHO INFO: Clean completed successfully!
