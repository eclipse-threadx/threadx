@echo off
rem ##########################################################################
rem Copyright (c) 2026 safecubed GmbH
rem Copyright (c) 2026 Eclipse ThreadX contributors
rem
rem This program and the accompanying materials are made available under the
rem terms of the MIT License which is available at
rem https://opensource.org/licenses/MIT.
rem
rem SPDX-License-Identifier: MIT
rem ##########################################################################

rem Portions of this file were generated with AI assistance.

rem Regression test build runner for the TriCore TC1.6.2 port on Windows.
rem Usage: run.bat build [all|<config> ...]
rem The AURIX GCC (tricore-elf-), CMake and Ninja shall be on PATH. The images
rem run on the TC375 Lite Kit, so this script only builds them. See README.md
rem for programming and running them.

setlocal EnableExtensions EnableDelayedExpansion
set "SCRIPT_NAME=%~nx0"
cd /d "%~dp0"
for %%I in ("%~dp0..\..\..\..") do set "REPO_ROOT=%%~fI"
set "REPO_ROOT=%REPO_ROOT:\=/%"

rem Parse the build configurations from the set(BUILD_CONFIGURATIONS ...)
rem command of CMakeLists.txt, which may span several lines.
set "CONFIGS="
set "IN_LIST="
for /f "usebackq delims=" %%L in ("CMakeLists.txt") do (
    set "LINE=%%L"
    if not defined IN_LIST if not "!LINE:set(BUILD_CONFIGURATIONS=!"=="!LINE!" (
        set "IN_LIST=1"
        set "LINE=!LINE:set(BUILD_CONFIGURATIONS=!"
    )
    if defined IN_LIST (
        set "LAST="
        if not "!LINE:)=!"=="!LINE!" set "LAST=1"
        set "LINE=!LINE:)= !"
        for %%W in (!LINE!) do (
            set "WORD=%%W"
            if not "!WORD:build=!"=="!WORD!" set "CONFIGS=!CONFIGS! %%W"
        )
        if defined LAST set "IN_LIST="
    )
)

if not "%~1"=="build" goto help
shift

set "BUILDS="
if "%~1"=="" (
    for %%C in (%CONFIGS%) do if not defined BUILDS set "BUILDS=%%C"
    goto run
)
if "%~1"=="all" if "%~2"=="" (
    set "BUILDS=%CONFIGS%"
    goto run
)

:args
if "%~1"=="" goto run
call :validate "%~1" || goto help
set "BUILDS=!BUILDS! %~1"
shift
goto args

rem A failing configuration does not stop the ones after it. The status is
rem accumulated and returned.
:run
set "BUILD_STATUS=0"
for %%B in (%BUILDS%) do (
    echo Building %%B
    call :generate %%B && call :build %%B || set "BUILD_STATUS=1"
    echo.
)
exit /b %BUILD_STATUS%

:help
echo Usage: %SCRIPT_NAME% build [all^|^<build_configuration^> ...]
echo.
echo Available build configurations:
for %%C in (%CONFIGS%) do echo   %%C
exit /b 1

:validate
for %%C in (%CONFIGS%) do if "%~1"=="%%C" exit /b 0
exit /b 1

:generate
cmake -B"build/%~1" -GNinja ^
    -DCMAKE_TOOLCHAIN_FILE="%REPO_ROOT%/cmake/tricore_tc1_6_2.cmake" ^
    -DCMAKE_BUILD_TYPE="%~1" ^
    .
exit /b %ERRORLEVEL%

:build
cmake --build "build/%~1"
exit /b %ERRORLEVEL%
