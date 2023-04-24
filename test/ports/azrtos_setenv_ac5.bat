@ECHO OFF

ECHO INFO: Setting up the environment for ARM Compiler v5...

IF EXIST "%AZRTOS_ARM_COMPILER_5_DIR%" GOTO FOUND_ARM_COMPILER_5

SET "AZRTOS_ARM_COMPILER_5_DIR=%ProgramFiles%\Arm\Development Studio 2021.0\sw\ARMCompiler5.06u7\bin"
IF EXIST "%AZRTOS_ARM_COMPILER_5_DIR%" (
    SET "PATH=%AZRTOS_ARM_COMPILER_5_DIR%;%PATH%"
    IF "%ARMLMD_LICENSE_FILE%"=="" (
        SET "ARMLMD_LICENSE_FILE=%APPDATA%\arm\ds\licenses"
    )
    SET "ARM_CONFIG_PATH=%APPDATA%\arm\ds\2021.0"
    SET "ARM_PRODUCT_DEF=%ProgramFiles%\Arm\Development Studio 2021.0\sw\mappings\gold.elmap"
    GOTO FOUND_ARM_COMPILER_5
)

SET "AZRTOS_ARM_COMPILER_5_DIR=%ProgramFiles%\Arm\Development Studio 2020.1\sw\ARMCompiler5.06u7\bin"
IF EXIST "%AZRTOS_ARM_COMPILER_5_DIR%" (
    SET "PATH=%AZRTOS_ARM_COMPILER_5_DIR%;%PATH%"
    IF "%ARMLMD_LICENSE_FILE%"=="" (
        SET "ARMLMD_LICENSE_FILE=%APPDATA%\arm\ds\licenses"
    )
    SET "ARM_CONFIG_PATH=%APPDATA%\arm\ds\2020.1"
    SET "ARM_PRODUCT_DEF=%ProgramFiles%\Arm\Development Studio 2020.1\sw\mappings\gold.elmap"
    GOTO FOUND_ARM_COMPILER_5
)

SET "AZRTOS_ARM_COMPILER_5_DIR=%ProgramFiles%\Arm\Development Studio 2020.0\sw\ARMCompiler5.06u6\bin"
IF EXIST "%AZRTOS_ARM_COMPILER_5_DIR%" (
    SET "PATH=%AZRTOS_ARM_COMPILER_5_DIR%;%PATH%"
    IF "%ARMLMD_LICENSE_FILE%"=="" (
        SET "ARMLMD_LICENSE_FILE=%APPDATA%\arm\ds\licenses"
    )
    SET "ARM_CONFIG_PATH=%APPDATA%\arm\ds\2020.0"
    SET "ARM_PRODUCT_DEF=%ProgramFiles%\Arm\Development Studio 2020.0\sw\mappings\gold.elmap"
    GOTO FOUND_ARM_COMPILER_5
)

SET "AZRTOS_ARM_COMPILER_5_DIR=%ProgramFiles%\DS-5 v5.29.3\sw\ARMCompiler5.06u6\bin"
IF EXIST "%AZRTOS_ARM_COMPILER_5_DIR%" (
    SET "PATH=%AZRTOS_ARM_COMPILER_5_DIR%;%PATH%"
    IF "%ARMLMD_LICENSE_FILE%"=="" (
        SET "ARMLMD_LICENSE_FILE=%APPDATA%\arm\ds\licenses"
    )
    SET "ARM_CONFIG_PATH=%APPDATA%\ARM\DS-5_v5.29.3"
    SET "ARM_PRODUCT_PATH=%ProgramFiles%\DS-5 v5.29.3\sw\mappings"
    GOTO FOUND_ARM_COMPILER_5
)

ECHO ERROR: ARM Compiler v5 not found!
EXIT /B 2

:FOUND_ARM_COMPILER_5
ECHO INFO: ARM Compiler v5 found in %AZRTOS_ARM_COMPILER_5_DIR%
ECHO INFO: ARM Compiler v5 environmnet setup completed successfully!
