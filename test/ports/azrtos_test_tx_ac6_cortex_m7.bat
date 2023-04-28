@echo off

DEL /Q test.log > NUL

ECHO INFO: Starting test...

%AZRTOS_ARM_DS_DIR%\bin\armdbg.exe --cdb-entry "Arm FVP::MPS2_Cortex_M7::Bare Metal Debug::Bare Metal Debug::Cortex-M7" --image sample_threadx/Debug/sample_threadx.axf --script %AZRTOS_TEST_DIR%\azrtos_test_tx_ac6_cortex_m7.ds
IF %ERRORLEVEL% NEQ 0 (
    ECHO ERROR: test execution failed.
    EXIT /B 1
)

COMP /A /L /M test.log %AZRTOS_TEST_DIR%\azrtos_test_tx_ac6_cortex_m7.log.expected
IF %ERRORLEVEL% NEQ 0 (
    ECHO ERROR: unexpected test results. 
    EXIT /B 1
)

ECHO INFO: Test completed without errors.
