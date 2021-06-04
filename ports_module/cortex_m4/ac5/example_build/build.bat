@ECHO OFF
ECHO Starting build...
CALL build_threadx.bat
CALL build_threadx_demo.bat
CALL build_threadx_module_library.bat
CALL build_threadx_module_demo.bat
CALL build_threadx_module_manager_demo.bat
ECHO Build finished.
