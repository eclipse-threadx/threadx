@ECHO OFF

DEL test.log > NUL

ECHO Testing %CD%...

%AZRTOS_ARM_DS_DIR%\bin\armdbg.exe --cdb-entry "Arm FVP::MPS2_Cortex_M4::Bare Metal Debug::Bare Metal Debug::Cortex-M4" --image Debug\Exe\sample_threadx.out --script %AZRTOS_TEST_DIR%\azrtos_test_tx_iar_cortex_m4_fvp.ds
IF %ERRORLEVEL% NEQ 0 (
    ECHO ERROR: test execution failed.
    EXIT /B 1
)

COMP /A /L /M test.log %AZRTOS_TEST_DIR%\azrtos_test_tx_iar_cortex_m4_fvp.log.expected
IF %ERRORLEVEL% NEQ 0 (
    ECHO ERROR: unexpected test results. 
    EXIT /B 1
)

ECHO INFO: Test completed successfully!
