@ECHO OFF

ECHO INFO: Building %CD%...

%AZRTOS_GHS_COMP_GBUILD% -top azure_rtos_workspace.gpj tx.a txe.a sample_threadx sample_threadx_el
IF %ERRORLEVEL% NEQ 0 (
    echo ERROR: build failed.
    EXIT /B 1
)

ECHO INFO: Build completed successfully!
