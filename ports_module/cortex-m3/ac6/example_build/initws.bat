@ECHO OFF

ECHO Initializing the workspace...

SETLOCAL ENABLEEXTENSIONS

%ARMDSIDEC% -nosplash -application org.eclipse.cdt.managedbuilder.core.headlessbuild -data .\workspace -import .\tx -import .\txm -import .\sample_threadx -import .\sample_threadx_module -import .\sample_threadx_module_manager 
IF %ERRORLEVEL% EQU 0 GOTO WS_INITIALIZED
ECHO ERROR: failed to initialize the workspace
EXIT /B 2

:WS_INITIALIZED
echo Workspace initialized.
EXIT /B 0
