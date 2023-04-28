@ECHO OFF

ECHO INFO: Starting test...

DEL test.log > NUL

%AZRTOS_ARM_DS_DBG% --cdb-entry "Arm FVP::MPS2_Cortex_M0::Bare Metal Debug::Bare Metal Debug::Cortex-M0" --image sample_threadx.axf --script %AZRTOS_TEST_DIR%\azrtos_test_tx_ac5_cortex_m0.ds
IF %ERRORLEVEL% NEQ 0 (
    ECHO ERROR: test execution failed.
    EXIT /B 1
)

COMP /A /L /M test.log %AZRTOS_TEST_DIR%\azrtos_test_tx_ac5_cortex_m0.log.expected
IF %ERRORLEVEL% NEQ 0 (
    ECHO ERROR: unexpected test results. 
    EXIT /B 1
)

ECHO INFO: Test completed successfully!
