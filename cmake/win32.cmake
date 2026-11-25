set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86)

set(THREADX_ARCH "win32")
set(THREADX_TOOLCHAIN "vs_2019")

# This makes the test compiles use the static library option so that the
# compiler environment can be discovered from the active MSVC shell.
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
