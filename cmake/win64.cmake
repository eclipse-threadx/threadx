set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR AMD64)

set(THREADX_ARCH "win64")
set(THREADX_TOOLCHAIN "vs_2022")

# This makes the test compiles use the static library option so that the
# compiler environment can be discovered from the active MSVC shell.
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
