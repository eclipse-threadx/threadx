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

rem Builds the ThreadX library tx.a and sample_threadx.elf for the AURIX TC375
rem Lite Kit without a driver library, as build_threadx_sample.sh does. The
rem start-up code is crt0-tc38xx.o of the AURIX GCC: the compiler has no TC37x
rem device, and the TC38x start-up code applies to the TC375 unchanged.
rem sample_threadx.ld replaces the memory description of the TC38x.
rem
rem CROSS_COMPILE selects the toolchain prefix. The default is the AURIX GCC
rem (tricore-elf-), which shall be on PATH.

setlocal EnableExtensions
cd /d "%~dp0"

if not defined CROSS_COMPILE set "CROSS_COMPILE=tricore-elf-"
set "CC=%CROSS_COMPILE%gcc"
rem tricore-elf-ar loads lib\bfd-plugins\libdep.a, which is no DLL.
set "AR=%CROSS_COMPILE%gcc-ar"

set "THREADX_ROOT=..\..\..\.."
set "BUILD_DIR=build"

set "CFLAGS=-mcpu=tc38xx -std=c99 -O0 -g3 -Wall -fmessage-length=0 -fno-common -fdata-sections -ffunction-sections"
set "INCLUDES=-I%THREADX_ROOT%/common/inc -I../inc"

if exist "%BUILD_DIR%" rmdir /s /q "%BUILD_DIR%"
mkdir "%BUILD_DIR%\tx" "%BUILD_DIR%\sample" || exit /b 1
del /q tx.a sample_threadx.elf sample_threadx.map 2>nul

rem The archiver reads the object list from a response file. The response file
rem uses forward slashes, because the archiver treats a backslash in it as an
rem escape character.
for %%f in (..\src\*.c %THREADX_ROOT%\common\src\*.c) do (
    "%CC%" %CFLAGS% %INCLUDES% -c "%%f" -o "%BUILD_DIR%/tx/%%~nf.o" || exit /b 1
    echo %BUILD_DIR%/tx/%%~nf.o>>"%BUILD_DIR%\tx.rsp"
)
"%AR%" -r tx.a @%BUILD_DIR%/tx.rsp || exit /b 1

for %%f in (sample_threadx.c tx_initialize_low_level.c) do (
    "%CC%" %CFLAGS% %INCLUDES% -c "%%f" -o "%BUILD_DIR%/sample/%%~nf.o" || exit /b 1
)

"%CC%" -mcpu=tc38xx -T sample_threadx.ld -Wl,--gc-sections -Wl,-Map=sample_threadx.map ^
    -o sample_threadx.elf %BUILD_DIR%/sample/sample_threadx.o ^
    %BUILD_DIR%/sample/tx_initialize_low_level.o tx.a || exit /b 1
