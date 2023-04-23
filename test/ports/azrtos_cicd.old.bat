@REM ***************************************************************************
@REM
@REM Builds/Cleans/Tests all examples
@REM
@REM Call this script without parameters for usage help.
@REM
@REM ***************************************************************************

@ECHO OFF

SETLOCAL ENABLEEXTENSIONS
SETLOCAL ENABLEDELAYEDEXPANSION

SET SCRIPT_NAME=%~n0%~x0
CALL :DBG_MSG "SCRIPT_NAME: %SCRIPT_NAME%"
SET "AZRTOS_TEST_DIR=%~dp0"
CALL :DBG_MSG "AZRTOS_TEST_DIR: %AZRTOS_TEST_DIR%"
SET "AZRTOS_REPO_DIR=%~dp0..\."
CALL :DBG_MSG "AZRTOS_REPO_DIR: %AZRTOS_REPO_DIR%"

CALL :DBG_MSG "Parsing parameters..."
IF /I "%1" EQU "" GOTO PRINT_USAGE_AND_EXIT
IF /I "%1" EQU "BUILD" SET FLAG_DO_WHAT=BUILD
IF /I "%1" EQU "CLEAN" SET FLAG_DO_WHAT=CLEAN
IF /I "%1" EQU "TEST" SET FLAG_DO_WHAT=TEST
IF "%FLAG_DO_WHAT%"=="" GOTO PRINT_USAGE_AND_EXIT
CALL :DBG_MSG "FLAG_DO_WHAT: %FLAG_DO_WHAT%"
SHIFT
IF /I "%1" EQU "" GOTO PRINT_USAGE_AND_EXIT
IF /I "%1" EQU "TX" SET FLAG_DO_WHICH=TX
IF /I "%1" EQU "TXM" SET FLAG_DO_WHICH=TXM
IF "%FLAG_DO_WHICH%"=="" GOTO PRINT_USAGE_AND_EXIT
CALL :DBG_MSG "FLAG_DO_WHICH: %FLAG_DO_WHICH%"
SET FLAG_GHS=0
SET FLAG_IAR=0
SET FLAG_AC5=0
SET FLAG_AC6=0
SET FLAG_KUV=0
SET FLAG_GNU=0
CALL :DBG_MSG "Parsing groups..."
:PARSE_GROUP_PARAMS
SHIFT
IF /I "%1" EQU "" GOTO GROUP_PARAMS_PARSED
IF /I "%1" EQU "ALL" (
    SET FLAG_IAR=1
    SET FLAG_GHS=1
    SET FLAG_AC5=1
    SET FLAG_AC6=1
    SET FLAG_KUV=1
    SET FLAG_GNU=1
)
IF /I "%1" EQU "IAR" SET FLAG_IAR=1
IF /I "%1" EQU "GHS" SET FLAG_GHS=1
IF /I "%1" EQU "AC5" SET FLAG_AC5=1
IF /I "%1" EQU "AC6" SET FLAG_AC6=1
IF /I "%1" EQU "KUV" SET FLAG_KUV=1
IF /I "%1" EQU "GNU" SET FLAG_GNU=1
GOTO PARSE_GROUP_PARAMS
CALL :DBG_MSG "Groups parsed."
:GROUP_PARAMS_PARSED
CALL :DBG_MSG "FLAG_IAR: %FLAG_IAR%"
CALL :DBG_MSG "FLAG_GHS: %FLAG_GHS%"
CALL :DBG_MSG "FLAG_AC5: %FLAG_AC5%"
CALL :DBG_MSG "FLAG_AC6: %FLAG_AC6%"
CALL :DBG_MSG "FLAG_KUV: %FLAG_KUV%"
CALL :DBG_MSG "FLAG_GNU: %FLAG_GNU%"
CALL :DBG_MSG "Parameters parsed."

IF %FLAG_IAR% EQU 1 (
    CALL azrtos_setenv_iar.bat
    IF %ERRORLEVEL% NEQ 0 GOTO :SETENV_ERROR
)
IF %FLAG_GHS% EQU 1 (
    CALL azrtos_setenv_ghs.bat
    IF %ERRORLEVEL% NEQ 0 GOTO :SETENV_ERROR
)
IF %FLAG_AC5% EQU 1 (
    CALL azrtos_setenv_ac5.bat
    IF %ERRORLEVEL% NEQ 0 GOTO :SETENV_ERROR
)
IF %FLAG_AC6% EQU 1 (
    CALL azrtos_setenv_ac6.bat
    IF %ERRORLEVEL% NEQ 0 GOTO :SETENV_ERROR
)
IF %FLAG_KUV% EQU 1 (
    CALL azrtos_setenv_kuv.bat
    IF %ERRORLEVEL% NEQ 0 GOTO :SETENV_ERROR
)
IF %FLAG_GNU% EQU 1 (
    CALL azrtos_setenv_gnu.bat
    IF %ERRORLEVEL% NEQ 0 GOTO :SETENV_ERROR
)
IF "%FLAG_DO_WHAT%" EQU "TEST" (
    IF %FLAG_GNU% EQU 1 (
        CALL azrtos_setenv_arm_dbg.bat
    )
)

SET LOGFILE=%AZRTOS_REPO_DIR%\test\azrtos_do_all.log
CALL :DBG_MSG "LOGFILE: %LOGFILE%"

SET /A DIR_COUNT=0
SET /A DIR_SUCCESS_COUNT=0
SET /A DIR_ERROR_COUNT=0

IF %FLAG_DO_WHICH%==TX (
    IF %FLAG_IAR%==1 (
        CALL :DO_DIR IAR CORTEX_M0 %AZRTOS_REPO_DIR%\ports\cortex_m0\iar\example_build
        CALL :DO_DIR IAR CORTEX_M3 %AZRTOS_REPO_DIR%\ports\cortex_m3\iar\example_build
        CALL :DO_DIR IAR CORTEX_M4 %AZRTOS_REPO_DIR%\ports\cortex_m4\iar\example_build
        CALL :DO_DIR IAR CORTEX_M7 %AZRTOS_REPO_DIR%\ports\cortex_m7\iar\example_build
        REM CALL :DO_DIR IAR CORTEX_M23 %AZRTOS_REPO_DIR%\ports\cortex_m23\iar\example_build
        REM CALL :DO_DIR IAR CORTEX_M33 %AZRTOS_REPO_DIR%\ports\cortex_m33\iar\example_build
        CALL :DO_DIR IAR CORTEX_R4 %AZRTOS_REPO_DIR%\ports\cortex_r4\iar\example_build
        CALL :DO_DIR IAR CORTEX_R5 %AZRTOS_REPO_DIR%\ports\cortex_r5\iar\example_build
        CALL :DO_DIR IAR CORTEX_A5 %AZRTOS_REPO_DIR%\ports\cortex_a5\iar\example_build
        CALL :DO_DIR IAR CORTEX_A7 %AZRTOS_REPO_DIR%\ports\cortex_a7\iar\example_build
        CALL :DO_DIR IAR CORTEX_A8 %AZRTOS_REPO_DIR%\ports\cortex_a8\iar\example_build
        CALL :DO_DIR IAR CORTEX_A9 %AZRTOS_REPO_DIR%\ports\cortex_a9\iar\example_build
        CALL :DO_DIR IAR CORTEX_A15 %AZRTOS_REPO_DIR%\ports\cortex_a15\iar\example_build
        CALL :DO_DIR IAR CORTEX_ARM9 %AZRTOS_REPO_DIR%\ports\arm9\iar\example_build
        CALL :DO_DIR IAR CORTEX_ARM11 %AZRTOS_REPO_DIR%\ports\arm11\iar\example_build
        REM CALL :DO_DIR IAR RISC_V32 %AZRTOS_REPO_DIR%\ports\risc-v32\iar\example_build
    )
    IF %FLAG_GHS%==1 (
        CALL :DO_DIR GHS CORTEX_M3 %AZRTOS_REPO_DIR%\ports\cortex_m3\ghs\example_build
        CALL :DO_DIR GHS CORTEX_M4 %AZRTOS_REPO_DIR%\ports\cortex_m4\ghs\example_build
        CALL :DO_DIR GHS CORTEX_M7 %AZRTOS_REPO_DIR%\ports\cortex_m7\ghs\example_build
        CALL :DO_DIR GHS CORTEX_R4 %AZRTOS_REPO_DIR%\ports\cortex_r4\ghs\example_build
        CALL :DO_DIR GHS CORTEX_R5 %AZRTOS_REPO_DIR%\ports\cortex_r5\ghs\example_build
        CALL :DO_DIR GHS CORTEX_R7 %AZRTOS_REPO_DIR%\ports\cortex_r7\ghs\example_build
        CALL :DO_DIR GHS CORTEX_A5 %AZRTOS_REPO_DIR%\ports\cortex_a5\ghs\example_build
        CALL :DO_DIR GHS CORTEX_A7 %AZRTOS_REPO_DIR%\ports\cortex_a7\ghs\example_build
        CALL :DO_DIR GHS CORTEX_A8 %AZRTOS_REPO_DIR%\ports\cortex_a8\ghs\example_build
        CALL :DO_DIR GHS CORTEX_A9 %AZRTOS_REPO_DIR%\ports\cortex_a9\ghs\example_build
    )
    IF %FLAG_AC5%==1 (
        CALL :DO_DIR AC5 CORTEX_M4 %AZRTOS_REPO_DIR%\ports\cortex_m4\ac5\example_build
    )
    IF %FLAG_AC6%==1 (
        CALL :DO_DIR AC6 CORTEX_M0 %AZRTOS_REPO_DIR%\ports\cortex_m0\ac6\example_build
        CALL :DO_DIR AC6 CORTEX_M3 %AZRTOS_REPO_DIR%\ports\cortex_m3\ac6\example_build
        CALL :DO_DIR AC6 CORTEX_M4 %AZRTOS_REPO_DIR%\ports\cortex_m4\ac6\example_build
        CALL :DO_DIR AC6 CORTEX_M7 %AZRTOS_REPO_DIR%\ports\cortex_m7\ac6\example_build
        REM CALL :DO_DIR AC6 CORTEX_M23 %AZRTOS_REPO_DIR%\ports\cortex_m23\ac6\example_build
        REM CALL :DO_DIR AC6 CORTEX_M33 %AZRTOS_REPO_DIR%\ports\cortex_m33\ac6\example_build
        CALL :DO_DIR AC6 CORTEX_R4 %AZRTOS_REPO_DIR%\ports\cortex_r4\ac6\example_build
        CALL :DO_DIR AC6 CORTEX_R5 %AZRTOS_REPO_DIR%\ports\cortex_r5\ac6\example_build
        CALL :DO_DIR AC6 CORTEX_A5X %AZRTOS_REPO_DIR%\ports\cortex_a5x\ac6\example_build
        CALL :DO_DIR AC6 CORTEX_A7 %AZRTOS_REPO_DIR%\ports\cortex_a7\ac6\example_build
        CALL :DO_DIR AC6 CORTEX_A8 %AZRTOS_REPO_DIR%\ports\cortex_a8\ac6\example_build
        CALL :DO_DIR AC6 CORTEX_A9 %AZRTOS_REPO_DIR%\ports\cortex_a9\ac6\example_build
        CALL :DO_DIR AC6 CORTEX_A15 %AZRTOS_REPO_DIR%\ports\cortex_a15\ac6\example_build
    )
    IF %FLAG_KUV%==1 (
        CALL :DO_DIR KUV CORTEX_M3 %AZRTOS_REPO_DIR%\ports\cortex_m3\keil\example_build
        CALL :DO_DIR KUV CORTEX_M4 %AZRTOS_REPO_DIR%\ports\cortex_m4\keil\example_build
    )
    IF %FLAG_GNU%==1 (
        CALL :DO_DIR GNU CORTEX_M0 %AZRTOS_REPO_DIR%\ports\cortex_m0\gnu\example_build
        CALL :DO_DIR GNU CORTEX_M3 %AZRTOS_REPO_DIR%\ports\cortex_m3\gnu\example_build
        CALL :DO_DIR GNU CORTEX_M4 %AZRTOS_REPO_DIR%\ports\cortex_m4\gnu\example_build
        CALL :DO_DIR GNU CORTEX_M7 %AZRTOS_REPO_DIR%\ports\cortex_m7\gnu\example_build
        REM CALL :DO_DIR GNU CORTEX_M23 %AZRTOS_REPO_DIR%\ports\cortex_m23\gnu\example_build
        REM CALL :DO_DIR GNU CORTEX_M33 %AZRTOS_REPO_DIR%\ports\cortex_m33\gnu\example_build
        CALL :DO_DIR GNU CORTEX_R4 %AZRTOS_REPO_DIR%\ports\cortex_r4\gnu\example_build
        CALL :DO_DIR GNU CORTEX_R5 %AZRTOS_REPO_DIR%\ports\cortex_r5\gnu\example_build
        CALL :DO_DIR GNU CORTEX_A5 %AZRTOS_REPO_DIR%\ports\cortex_a5\gnu\example_build
        CALL :DO_DIR GNU CORTEX_A7 %AZRTOS_REPO_DIR%\ports\cortex_a7\gnu\example_build
        CALL :DO_DIR GNU CORTEX_A8 %AZRTOS_REPO_DIR%\ports\cortex_a8\gnu\example_build
        CALL :DO_DIR GNU CORTEX_A9 %AZRTOS_REPO_DIR%\ports\cortex_a9\gnu\example_build
        CALL :DO_DIR GNU CORTEX_A15 %AZRTOS_REPO_DIR%\ports\cortex_a15\gnu\example_build
        REM CALL :DO_DIR GNU CORTEX_A35 %AZRTOS_REPO_DIR%\ports\cortex_a35\gnu\example_build
    )
)
IF %FLAG_DO_WHICH%==TXM (
    IF %FLAG_IAR%==1 (
        SET CURRENT_TOOLCHAIN=IAR
        CALL :DO_DIR IAR CORTEX_M3 %AZRTOS_REPO_DIR%\ports_module\cortex-m3\iar\example_build
        CALL :DO_DIR IAR CORTEX_M4 %AZRTOS_REPO_DIR%\ports_module\cortex-m4\iar\example_build
        CALL :DO_DIR IAR CORTEX_M7 %AZRTOS_REPO_DIR%\ports_module\cortex-m7\iar\example_build
        CALL :DO_DIR IAR CORTEX_R4 %AZRTOS_REPO_DIR%\ports_module\cortex-r4\iar\example_build
    )
    IF %FLAG_AC5%==1 (
        CALL :DO_DIR AC5 CORTEX_M3 %AZRTOS_REPO_DIR%\ports_module\cortex-m3\ac5\example_build
        CALL :DO_DIR AC5 CORTEX_M4 %AZRTOS_REPO_DIR%\ports_module\cortex-m4\ac5\example_build
        CALL :DO_DIR AC5 CORTEX_M7 %AZRTOS_REPO_DIR%\ports_module\cortex-m7\ac5\example_build
        CALL :DO_DIR AC5 CORTEX_A7 %AZRTOS_REPO_DIR%\ports_module\cortex-a7\ac5\example_build
    )
    IF %FLAG_AC6%==1 (
        CALL :DO_DIR AC6 CORTEX_M3 %AZRTOS_REPO_DIR%\ports_module\cortex-m3\ac6\example_build
        CALL :DO_DIR AC6 CORTEX_M4 %AZRTOS_REPO_DIR%\ports_module\cortex-m4\ac6\example_build
        CALL :DO_DIR AC6 CORTEX_M7 %AZRTOS_REPO_DIR%\ports_module\cortex-m7\ac6\example_build
        CALL :DO_DIR AC6 CORTEX_R4 %AZRTOS_REPO_DIR%\ports_module\cortex-r4\ac6\example_build
    )
    IF %FLAG_KUV%==1 (
        CALL :DO_DIR KUV CORTEX_M4 %AZRTOS_REPO_DIR%\ports_module\cortex-m4\keil\example_build
    )
    IF %FLAG_GNU%==1 (
        CALL :DO_DIR GNU CORTEX_M3 %AZRTOS_REPO_DIR%\ports_module\cortex-m3\gnu\example_build
        CALL :DO_DIR GNU CORTEX_M4 %AZRTOS_REPO_DIR%\ports_module\cortex-m4\gnu\example_build
        CALL :DO_DIR GNU CORTEX_M7 %AZRTOS_REPO_DIR%\ports_module\cortex-m7\gnu\example_build
    )
)
CALL :DBG_MSG "All operations called."

CALL :DBG_MSG "Printing results."
CALL :OUT_MSG "INFO: %FLAG_DO_WHAT% results:"
CALL :OUT_MSG "INFO: %DIR_COUNT% total."
CALL :OUT_MSG "INFO: %DIR_SUCCESS_COUNT% completed successfully." 
IF "%DIR_ERROR_COUNT%" NEQ "0" CALL :OUT_MSG "ERROR: %DIR_ERROR_COUNT% failed."
CALL :OUT_MSG "INFO: Ending %FLAG_DO_WHAT% on %DATE% at %TIME%."
EXIT /B 0


:PRINT_USAGE_AND_EXIT
ECHO.
ECHO Usage:
ECHO %SCRIPT_NAME% {build^|clean^|test} {tx^|txm} [[group] [[group] [...]]]
ECHO Performs the selected operation on ThreadX or ThreadX Modules examples in one or more groups,
ECHO where group is one of the following:
ECHO ALL -- ALL examples
ECHO IAR -- IAR Embedded Workbench examples
ECHO GHS -- Green Hills Software examples
ECHO AC5 -- ARM Developer Studio 5 examples
ECHO AC6 -- ARM Developer Studio 6 examples
ECHO KUV -- KEIL uVision
ECHO GNU -- GNU Compiler Collection
ECHO If no parameters are passed usage information is displayed.
ECHO Outputs a report to standard output.
ECHO Creates a log file with all output.
ECHO.
EXIT /B 0


:SETENV_ERROR
CALL :OUT_MSG "ERROR: failed to set the environment"
EXIT 1


:DO_DIR
SET "CURRENT_TOOLCHAIN=%~1"
SET "CURRENT_PLATFORM=%~2"
SET "CURRENT_DIRECTORY=%~3"
CALL :DBG_MSG "CURRENT_DIRECTORY: %CURRENT_DIRECTORY%" 
IF NOT EXIST %CURRENT_DIRECTORY% (
    EXIT /B 1
    CALL :OUT_MSG "ERROR: Directory doesn't exist: CURRENT_DIRECTORY"
)
PUSHD %CURRENT_DIRECTORY%
IF /I "%FLAG_DO_WHAT%" EQU "BUILD" SET CURRENT_SCRIPT=%AZRTOS_REPO_DIR%\test\azrtos_build_%FLAG_DO_WHICH%_%CURRENT_TOOLCHAIN%.bat
IF /I "%FLAG_DO_WHAT%" EQU "CLEAN" SET CURRENT_SCRIPT=%AZRTOS_REPO_DIR%\test\azrtos_clean_%FLAG_DO_WHICH%_%CURRENT_TOOLCHAIN%.bat
IF /I "%FLAG_DO_WHAT%" EQU "TEST" SET CURRENT_SCRIPT=%AZRTOS_REPO_DIR%\test\azrtos_test_%FLAG_DO_WHICH%_%CURRENT_TOOLCHAIN%_%CURRENT_PLATFORM%.bat
IF DEFINED DEBUG ECHO DEBUG: %CURRENT_SCRIPT%
IF NOT EXIST %CURRENT_SCRIPT% (
    CALL :OUT_MSG "ERROR: script does not exist: %CURRENT_SCRIPT%"
) ELSE (
    CALL :OUT_MSG "INFO: STARTING: %FLAG_DO_WHAT%: %CD% - %DATE% %TIME%"
    SET /A DIR_COUNT+=1
    CALL %CURRENT_SCRIPT% >> %LOGFILE% 2>&1
    IF %ERRORLEVEL% EQU 0 (
        CALL :OUT_MSG "INFO: SUCCESS:  %FLAG_DO_WHAT%: %CD% - %DATE% %TIME%"
        SET /A DIR_SUCCESS_COUNT+=1
    ) ELSE (
        CALL :OUT_MSG "ERROR:          %FLAG_DO_WHAT%: %CD% - %DATE% %TIME%"
        SET /A DIR_ERROR_COUNT+=1
    )
    POPD
)
EXIT /B 0


:DBG_MSG
IF DEFINED AZRTOS_DEBUG (ECHO DEBUG: %~1) 
EXIT /B 0


:OUT_MSG
ECHO %~1
IF DEFINED LOGFILE (ECHO %~1 > %LOGFILE%)
EXIT /B 0
