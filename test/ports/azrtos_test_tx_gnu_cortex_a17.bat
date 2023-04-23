@ECHO OFF

ECHO INFO: Starting test...

DEL test.log > NUL

%AZRTOS_ARM_DS_DBG% --cdb-entry "Arm FVP::VE_Cortex_A17x1::Bare Metal Debug::Bare Metal Debug::Cortex-A17" --image sample_threadx.out --script %AZRTOS_TEST_DIR%\azrtos_test_tx_gnu_cortex_a17.ds
IF %ERRORLEVEL% NEQ 0 (
    ECHO ERROR: test execution failed.
    EXIT /B 1
)

COMP /A /L /M test.log %AZRTOS_TEST_DIR%\azrtos_test_tx_gnu_cortex_a17.log.expected
IF %ERRORLEVEL% NEQ 0 (
    ECHO ERROR: unexpected test results. 
    EXIT /B 1
)

ECHO INFO: Test completed successfully!
