#
# Copyright (c) 2026 Eclipse ThreadX contributors
#
# This program and the accompanying materials are made available under the
# terms of the MIT License which is available at
# https://opensource.org/licenses/MIT.
#
# SPDX-License-Identifier: MIT
#

cmake_minimum_required(VERSION 3.16)

# Generate compile_commands.json for clangd and IDE language servers
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Enforce Cross-Compilation Toolchain if not explicitly specified
if(NOT CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/esp32c6-toolchain.cmake")
endif()

project(esp32c6_threadx_example C ASM)

# ------------------------------------------------------------------------------
# 1. ESP-IDF SDK Path Resolution (Cross-Platform: Windows & Linux)
# ------------------------------------------------------------------------------
if(ESP_IDF_PATH)
    # User specified -DESP_IDF_PATH=/path/to/esp-idf on CMake command line
    set(ESP_IDF_PATH "${ESP_IDF_PATH}")
elseif(DEFINED ENV{IDF_PATH})
    # Standard Espressif environment variable set via export.sh / export.ps1
    set(ESP_IDF_PATH "$ENV{IDF_PATH}")
else()
    # Search standard Espressif installation directories across OSes without hardcoding version numbers
    file(GLOB ESP_IDF_DEFAULT_PATHS
        "C:/esp/*/esp-idf"
        "C:/esp/esp-idf"
        "$ENV{USERPROFILE}/esp/*/esp-idf"
        "$ENV{USERPROFILE}/esp/esp-idf"
        "$ENV{HOME}/esp/*/esp-idf"
        "$ENV{HOME}/esp/esp-idf"
    )
    list(LENGTH ESP_IDF_DEFAULT_PATHS PATH_COUNT)
    if(PATH_COUNT GREATER 0)
        list(GET ESP_IDF_DEFAULT_PATHS 0 ESP_IDF_PATH)
    else()
        message(FATAL_ERROR "ESP-IDF SDK path not found. Please set the IDF_PATH environment variable or pass -DESP_IDF_PATH=/path/to/esp-idf.")
    endif()
endif()

message(STATUS "Using ESP-IDF hardware headers from: ${ESP_IDF_PATH}")

# ------------------------------------------------------------------------------
# 2. Include Directories
# ------------------------------------------------------------------------------
# ESP32-C6 Hardware & SoC Component Headers
set(ESP32C6_IDF_INCLUDES
    "${ESP_IDF_PATH}/components/soc/esp32c6/include"
    "${ESP_IDF_PATH}/components/soc/esp32c6/register"
    "${ESP_IDF_PATH}/components/soc/include"
    "${ESP_IDF_PATH}/components/hal/include"
    "${ESP_IDF_PATH}/components/hal/esp32c6/include"
    "${ESP_IDF_PATH}/components/hal/platform_port/include"
    "${ESP_IDF_PATH}/components/esp_hal_uart/include"
    "${ESP_IDF_PATH}/components/esp_hal_uart/esp32c6/include"
    "${ESP_IDF_PATH}/components/esp_rom/include"
    "${ESP_IDF_PATH}/components/esp_rom/esp32c6/include"
    "${ESP_IDF_PATH}/components/esp_common/include"
    "${ESP_IDF_PATH}/components/esp_app_format/include"
    "${ESP_IDF_PATH}/components/bootloader_support/include"
    "${ESP_IDF_PATH}/components/riscv/include"
)

# ThreadX Core & Port Include Directories
set(THREADX_INCLUDES
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../common/inc"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../inc"
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

include_directories(
    ${ESP32C6_IDF_INCLUDES}
    ${THREADX_INCLUDES}
)

# ------------------------------------------------------------------------------
# 3. ThreadX Static Library Target
# ------------------------------------------------------------------------------
file(GLOB THREADX_COMMON_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../../../../common/src/*.c")
file(GLOB THREADX_PORT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../../src/*.S")

add_library(threadx STATIC
    ${THREADX_COMMON_SOURCES}
    ${THREADX_PORT_SOURCES}
)

target_include_directories(threadx PUBLIC ${THREADX_INCLUDES})

# ------------------------------------------------------------------------------
# 4. ESP32-C6 Board Support Object Library (Assembly Startup & Pinned HAL)
# ------------------------------------------------------------------------------
set(ESP32C6_BOARD_BSP_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/entry.S"
    "${CMAKE_CURRENT_SOURCE_DIR}/vectors.S"
    "${CMAKE_CURRENT_SOURCE_DIR}/tx_initialize_low_level.S"
    "${CMAKE_CURRENT_SOURCE_DIR}/platform/console.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/platform/interrupt.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/platform/systimer.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/platform/startup.c"
    "${ESP_IDF_PATH}/components/hal/systimer_hal.c"
)

add_library(esp32c6_bsp OBJECT
    ${ESP32C6_BOARD_BSP_SOURCES}
)
target_include_directories(esp32c6_bsp PUBLIC ${THREADX_INCLUDES} ${ESP32C6_IDF_INCLUDES})

# ------------------------------------------------------------------------------
# 5. Linker Configuration
# ------------------------------------------------------------------------------
set(ESP32C6_LINKER_SEARCH_PATHS
    "${ESP_IDF_PATH}/components/soc/esp32c6/ld"
    "${ESP_IDF_PATH}/components/esp_rom/esp32c6/ld"
)

set(ESP32C6_LINK_FLAGS "-T${CMAKE_CURRENT_SOURCE_DIR}/link.ld")
foreach(LD_PATH ${ESP32C6_LINKER_SEARCH_PATHS})
    string(APPEND ESP32C6_LINK_FLAGS " -L${LD_PATH}")
endforeach()

# ------------------------------------------------------------------------------
# 6. Executable Firmware Target & Image Generation (.elf -> .bin)
# ------------------------------------------------------------------------------
add_executable(esp32c6_demo.elf
    "${CMAKE_CURRENT_SOURCE_DIR}/demo_threadx.c"
    $<TARGET_OBJECTS:esp32c6_bsp>
)

target_include_directories(esp32c6_demo.elf PUBLIC ${THREADX_INCLUDES} ${ESP32C6_IDF_INCLUDES})

target_link_options(esp32c6_demo.elf PRIVATE
    "-T${CMAKE_CURRENT_SOURCE_DIR}/link.ld"
    "-L${ESP_IDF_PATH}/components/soc/esp32c6/ld"
    "-L${ESP_IDF_PATH}/components/esp_rom/esp32c6/ld"
)

target_link_libraries(esp32c6_demo.elf PRIVATE threadx)

# Locate Python interpreter for esptool.py image conversion
find_package(Python3 COMPONENTS Interpreter REQUIRED)

add_custom_command(TARGET esp32c6_demo.elf POST_BUILD
    COMMAND ${Python3_EXECUTABLE} "${ESP_IDF_PATH}/components/esptool_py/esptool/esptool.py"
            --chip esp32c6 elf2image -o "${CMAKE_CURRENT_BINARY_DIR}/esp32c6_demo.bin"
            "${CMAKE_CURRENT_BINARY_DIR}/esp32c6_demo.elf"
    COMMENT "Generating bootable ESP32-C6 binary image: esp32c6_demo.bin"
)

message(STATUS "ESP32-C6 ThreadX complete build system ready.")
