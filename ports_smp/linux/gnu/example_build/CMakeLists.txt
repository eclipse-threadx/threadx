# Build the sample_threadx program that ports/linux/gnu/Makefile builds, so the
# CMake build can produce a runnable demo rather than only the library.
#
# Guarded by THREADX_SAMPLE in the parent so the default build stays a pure
# library build.

add_executable(sample_threadx)

target_sources(sample_threadx PRIVATE ${CMAKE_CURRENT_LIST_DIR}/sample_threadx.c)

target_include_directories(sample_threadx
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../inc
    ${CMAKE_CURRENT_LIST_DIR}/../../../../${TX_COMMON_DIR}/inc
)

target_link_libraries(sample_threadx PRIVATE threadx)
