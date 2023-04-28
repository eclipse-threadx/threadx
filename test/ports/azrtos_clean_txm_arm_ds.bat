@ECHO OFF
ECHO INFO: Cleaning %CD%...
RMDIR /Q /S workspace
RMDIR /Q /S sample_threadx\.settings
RMDIR /Q /S sample_threadx\Debug
RMDIR /Q /S sample_threadx\Release
RMDIR /Q /S sample_threadx_module\.settings
RMDIR /Q /S sample_threadx_module\Debug
RMDIR /Q /S sample_threadx_module\Release
RMDIR /Q /S sample_threadx_module_manager\.settings
RMDIR /Q /S sample_threadx_module_manager\Debug
RMDIR /Q /S sample_threadx_module_manager\Release
RMDIR /Q /S tx\.settings
RMDIR /Q /S tx\Debug
RMDIR /Q /S tx\Release
RMDIR /Q /S txm\.settings
RMDIR /Q /S txm\Debug
RMDIR /Q /S txm\Release
ECHO INFO: Cleaning completed successfully!
