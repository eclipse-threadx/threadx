@ECHO OFF

ECHO INFO: Debugging %CD%...

IF NOT EXIST workspace (
    ECHO ERROR: the workspace doesn't exist!
    EXIT /B 2
)

%AZRTOS_ARM_DS_IDEC% -nosplash --launcher.suppressErrors -data .\workspace 
IF %ERRORLEVEL% NEQ 0 (
    ECHO ERROR: build failed.
    EXIT /B 1
)

ECHO INFO: Debugger started successfully!
