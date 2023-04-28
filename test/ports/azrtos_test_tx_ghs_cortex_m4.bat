@ECHO OFF

ECHO Testing %CD%...

DEL test.log > NUL

%AZRTOS_GHS_MULTI_MULTI% -connectfile sample_threadx.con -connect="Simulator" -p %AZRTOS_TEST_DIR%\azrtos_test_tx_ghs_cortex_m4.rc sample_threadx
IF %ERRORLEVEL% NEQ 0 (
    ECHO ERROR: the test failed to run
    EXIT /B 1
)

COMP test.log %AZRTOS_TEST_DIR%\azrtos_test_tx_ghs_cortex_m4.valid.log /M /L /A
IF %ERRORLEVEL% NEQ 0 (
    ECHO Test results are invalid!
    EXIT /B 1
)

ECHO Test completed sucessfully!
