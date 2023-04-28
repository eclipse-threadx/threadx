@ECHO OFF

ECHO INFO: Building %CD%...

CALL build_threadx.bat
IF NOT EXIST tx.a (
    ECHO ERROR: ThreadX library build failed.
    EXIT /B 1
)

CALL build_threadx_sample.bat
IF NOT EXIST sample_threadx.out (
    ECHO ERROR: ThreadX example build failed.
    EXIT /B 1
)

ECHO INFO: Build completed successfully!
