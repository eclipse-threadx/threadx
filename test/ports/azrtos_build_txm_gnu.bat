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

CALL build_threadx_module_library.bat
IF NOT EXIST txm.a (
    ECHO ERROR: ThreadX module library build failed.
    EXIT /B 1
)

CALL build_threadx_module_sample.bat
IF NOT EXIST sample_threadx_module.out (
    ECHO ERROR: ThreadX module example build failed.
    EXIT /B 1
)

CALL build_threadx_module_manager_sample.bat
IF NOT EXIST sample_threadx_module_manager.out (
    ECHO ERROR: ThreadX module manager example build failed.
    EXIT /B 1
)

ECHO INFO: Build completed successfully!
