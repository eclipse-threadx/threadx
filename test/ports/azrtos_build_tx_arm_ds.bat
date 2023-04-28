@ECHO OFF

ECHO INFO: Building %CD%...

IF EXIST workspace GOTO WORKSPACE_EXISTS

ECHO INFO: Initializing the workspace...
%AZRTOS_ARM_DS_IDEC% -nosplash --launcher.suppressErrors -application org.eclipse.cdt.managedbuilder.core.headlessbuild -data .\workspace -import .\tx -import .\sample_threadx
IF %ERRORLEVEL% NEQ 0 (
    ECHO ERROR: failed to initialize the workspace
    EXIT /B 1
) ELSE (
    ECHO INFO: Workspace initialized.
)

:WORKSPACE_EXISTS
%AZRTOS_ARM_DS_IDEC% -nosplash --launcher.suppressErrors -application org.eclipse.cdt.managedbuilder.core.headlessbuild -data .\workspace -build tx -build sample_threadx
IF %ERRORLEVEL% NEQ 0 (
    ECHO ERROR: build failed.
    EXIT /B 1
)

ECHO INFO: Build completed successfully!
