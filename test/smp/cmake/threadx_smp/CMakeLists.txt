cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

# Set up the project
project(threadx_smp
    VERSION 6.0.0 
    LANGUAGES C ASM
)

if(NOT DEFINED THREADX_ARCH)
    message(FATAL_ERROR "Error: THREADX_ARCH not defined")
endif()
if(NOT DEFINED THREADX_TOOLCHAIN)
    message(FATAL_ERROR "Error: THREADX_TOOLCHAIN not defined")
endif()

set(PROJECT_DIR ${CMAKE_CURRENT_LIST_DIR}/../../../..)

# Define our target library and an alias for consumers
add_library(${PROJECT_NAME})
add_library("azrtos::${PROJECT_NAME}" ALIAS ${PROJECT_NAME})

# A place for generated/copied include files (no need to change)
set(CUSTOM_INC_DIR ${CMAKE_CURRENT_BINARY_DIR}/custom_inc)

# Pick up the port specific variables and apply them
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/ports_smp/${THREADX_ARCH}/${THREADX_TOOLCHAIN})

# Pick up the common stuff
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/common_smp)




# If the user provided an override, copy it to the custom directory
if (NOT TX_USER_FILE)
    message(STATUS "Using default tx_user.h file")
    set(TX_USER_FILE ${PROJECT_DIR}/common_smp/inc/tx_user_sample.h)
else()
    message(STATUS "Using custom tx_user.h file from ${TX_USER_FILE}")
endif()    
configure_file(${TX_USER_FILE} ${CUSTOM_INC_DIR}/tx_user.h COPYONLY)
target_include_directories(${PROJECT_NAME} 
    PUBLIC 
    ${CUSTOM_INC_DIR}
)
target_compile_definitions(${PROJECT_NAME} PUBLIC "TX_INCLUDE_USER_DEFINE_FILE" )

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