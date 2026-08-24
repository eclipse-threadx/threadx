set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# Let the compiler be chosen on the command line or from the environment, for example
# -DCMAKE_C_COMPILER=gcc-14 or CC=gcc-14. CMake reads a toolchain file before it consults
# CC and CXX, so setting these unconditionally would quietly override both and leave no
# way to build with anything but the distribution's default gcc.
if(NOT DEFINED CMAKE_C_COMPILER)
  if(DEFINED ENV{CC})
    set(CMAKE_C_COMPILER $ENV{CC})
  else()
    set(CMAKE_C_COMPILER gcc)
  endif()
endif()

if(NOT DEFINED CMAKE_CXX_COMPILER)
  if(DEFINED ENV{CXX})
    set(CMAKE_CXX_COMPILER $ENV{CXX})
  else()
    set(CMAKE_CXX_COMPILER g++)
  endif()
endif()
set(AS                  as)
set(AR                  ar)
set(OBJCOPY             objcopy)
set(OBJDUMP             objdump)
set(SIZE                size)

set(THREADX_ARCH "linux")
set(THREADX_TOOLCHAIN "gnu")

set(LINUX_FLAGS "-g -pthread")

set(CMAKE_C_FLAGS   "${LINUX_FLAGS} " CACHE INTERNAL "c compiler flags")
set(CMAKE_CXX_FLAGS "${LINUX_FLAGS} -fno-rtti -fno-exceptions" CACHE INTERNAL "cxx compiler flags")
set(CMAKE_ASM_FLAGS "${LINUX_FLAGS} -x assembler-with-cpp" CACHE INTERNAL "asm compiler flags")
set(CMAKE_EXE_LINKER_FLAGS "${LINUX_FLAGS} ${LD_FLAGS} -Wl,--gc-sections" CACHE INTERNAL "exe link flags")

SET(CMAKE_C_FLAGS_DEBUG "-Og -g -ggdb3" CACHE INTERNAL "c debug compiler flags")
SET(CMAKE_CXX_FLAGS_DEBUG "-Og -g -ggdb3" CACHE INTERNAL "cxx debug compiler flags")
SET(CMAKE_ASM_FLAGS_DEBUG "-g -ggdb3" CACHE INTERNAL "asm debug compiler flags")

SET(CMAKE_C_FLAGS_RELEASE "-O3" CACHE INTERNAL "c release compiler flags")
SET(CMAKE_CXX_FLAGS_RELEASE "-O3" CACHE INTERNAL "cxx release compiler flags")
SET(CMAKE_ASM_FLAGS_RELEASE "" CACHE INTERNAL "asm release compiler flags")

# this makes the test compiles use static library option so that we don't need to pre-set linker flags and scripts
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
