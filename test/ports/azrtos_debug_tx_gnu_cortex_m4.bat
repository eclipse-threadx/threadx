@echo off

ECHO INFO: Debugging %CD%...

%AZRTOS_ARM_DS_DBG% --cdb-entry "Arm FVP::MPS2_Cortex_M4::Bare Metal Debug::Bare Metal Debug::Cortex-M4" --image sample_threadx.out --script %AZRTOS_TEST_DIR%\azrtos_test_tx_gnu_cortex_m4.ds
IF %ERRORLEVEL% NEQ 0 (
    ECHO ERROR: test execution failed.
    EXIT /B 1
)

ECHO INFO: Debugger started successfully!
