@ECHO OFF
ECHO INFO: Cleaning %CD%...
%AZRTOS_GHS_COMP_GBUILD% -top azure_rtos_workspace.gpj -clean tx.a txe.a sample_threadx sample_threadx_el
IF %ERRORLEVEL% NEQ 0 (
    ECHO ERROR: clean failed.
    EXIT /B 1
) 
RMDIR /Q /S libs
DEL /Q /F test.log
ECHO INFO: Clean completed successfully!
