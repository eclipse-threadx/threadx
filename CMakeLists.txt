cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

if(NOT DEFINED THREADX_ARCH)
    message(FATAL_ERROR "Error: THREADX_ARCH not defined")
endif()
if(NOT DEFINED THREADX_TOOLCHAIN)
    message(FATAL_ERROR "Error: THREADX_TOOLCHAIN not defined")
endif()

# The Windows simulation ports build cleanly without executable try-compiles.
if((THREADX_ARCH STREQUAL "win32") OR (THREADX_ARCH STREQUAL "win64"))
    set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
endif()

# Set up the project. The Windows simulation ports do not use assembly and
# avoiding ASM language enablement keeps MSVC configuration on the CLI path
# deterministic.
if((THREADX_ARCH STREQUAL "win32") OR (THREADX_ARCH STREQUAL "win64"))
    project(threadx
        LANGUAGES C
    )
else()
    project(threadx
        LANGUAGES C ASM
    )
endif()

option(THREADX_SMP "Build ThreadX SMP version" OFF)

if(THREADX_SMP)
    set(TX_PORT_DIR "ports_smp")
    set(TX_COMMON_DIR "common_smp")
    set(TX_ARCH_DIR "${THREADX_ARCH}_smp")
    message(STATUS "Building ThreadX SMP version")
else()
    set(TX_PORT_DIR "ports")
    set(TX_COMMON_DIR "common")
    set(TX_ARCH_DIR "${THREADX_ARCH}")
    message(STATUS "Building standard ThreadX version")
endif()

message(STATUS "THREADX_ARCH: ${THREADX_ARCH}")
message(STATUS "THREADX_TOOLCHAIN: ${THREADX_TOOLCHAIN}")

# Define our target library and an alias for consumers
add_library(${PROJECT_NAME})
add_library("azrtos::${PROJECT_NAME}" ALIAS ${PROJECT_NAME})

# A place for generated/copied include files (no need to change)
set(CUSTOM_INC_DIR ${CMAKE_CURRENT_BINARY_DIR}/custom_inc)

# Pick up the port specific variables and apply them
if(DEFINED THREADX_CUSTOM_PORT)
    add_subdirectory(${THREADX_CUSTOM_PORT} threadx_port)
else()
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/${TX_PORT_DIR}/${TX_ARCH_DIR}/${THREADX_TOOLCHAIN})
endif()

# Pick up the common stuff
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/${TX_COMMON_DIR})

# Define the FreeRTOS adaptation layer
add_library(freertos-threadx EXCLUDE_FROM_ALL)
target_include_directories(freertos-threadx
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/utility/rtos_compatibility_layers/FreeRTOS
)
target_sources(freertos-threadx
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/utility/rtos_compatibility_layers/FreeRTOS/tx_freertos.c
)
target_link_libraries(freertos-threadx PUBLIC threadx)

# If the user provided an override, copy it to the custom directory
if (NOT TX_USER_FILE)
    message(STATUS "Using default tx_user.h file")
    set(TX_USER_FILE ${CMAKE_CURRENT_LIST_DIR}/${TX_COMMON_DIR}/inc/tx_user_sample.h)
else()
    message(STATUS "Using custom tx_user.h file from ${TX_USER_FILE}")
endif()
configure_file(${TX_USER_FILE} ${CUSTOM_INC_DIR}/tx_user.h COPYONLY)
target_include_directories(${PROJECT_NAME}
    PUBLIC
    ${CUSTOM_INC_DIR}
)
target_compile_definitions(${PROJECT_NAME} PUBLIC "TX_INCLUDE_USER_DEFINE_FILE" )
if(THREADX_SMP)
    target_compile_definitions(${PROJECT_NAME} PUBLIC "TX_MPCORE" )
endif()

# Optional: build for S-mode (Supervisor mode) instead of M-mode (Machine mode).
# Required when running after OpenSBI, e.g. booted from U-Boot.
option(TX_RISCV_SMODE "Use S-mode CSRs instead of M-mode for RISC-V targets" OFF)
if(TX_RISCV_SMODE)
    message(STATUS "RISC-V S-mode enabled (TX_RISCV_SMODE)")
    target_compile_definitions(${PROJECT_NAME} PUBLIC "TX_RISCV_SMODE")
endif()

# Enable a build target that produces a ZIP file of all sources
set(CPACK_SOURCE_GENERATOR "ZIP")
set(CPACK_SOURCE_IGNORE_FILES
  \\.git/
  \\.github/
  _build/
  \\.git
  \\.gitattributes
  \\.gitignore
  ".*~$"
)
set(CPACK_VERBATIM_VARIABLES YES)
include(CPack)
