@ECHO OFF
ECHO INFO: Cleaning %CD%...
RMDIR /Q /S workspace
RMDIR /Q /S sample_threadx\.settings
RMDIR /Q /S sample_threadx\Debug
RMDIR /Q /S sample_threadx\Release
RMDIR /Q /S tx\.settings
RMDIR /Q /S tx\Debug
RMDIR /Q /S tx\Release
ECHO INFO: Cleaning completed successfully!
