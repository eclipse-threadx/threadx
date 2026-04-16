set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR xtensa)

set(CMAKE_C_COMPILER    xt-clang)
set(CMAKE_CXX_COMPILER  xt-clang++)
set(AS                  xt-clang)
set(AR                  xt-ar)
set(OBJCOPY             xt-objcopy)
set(OBJDUMP             xt-objdump)
set(SIZE                xt-size)

set(THREADX_ARCH "xtensa")
set(THREADX_TOOLCHAIN "xcc")

if (DEFINED ENV{XCC_OPTS})
  set(XCC_OPTS $ENV{XCC_OPTS})
else()
  set(XCC_OPTS "")
endif()

set(XCC_FLAGS "${XCC_OPTS} -mlongcalls -mno-l32r-flix -mno-coproc -Wall -Wextra -Werror")

set(CMAKE_C_FLAGS   "${XCC_FLAGS} -Os -g" CACHE INTERNAL "c compiler flags")
set(CMAKE_CXX_FLAGS "${XCC_FLAGS} -Os -g -fno-rtti -fno-exceptions" CACHE INTERNAL "cxx compiler flags")
set(CMAKE_ASM_FLAGS "${XCC_FLAGS} -Os -g" CACHE INTERNAL "asm compiler flags")
set(CMAKE_EXE_LINKER_FLAGS "${XCC_FLAGS} ${LD_FLAGS} -Wl,--gc-sections" CACHE INTERNAL "exe link flags")

SET(CMAKE_C_FLAGS_DEBUG   "-O0 -g" CACHE INTERNAL "c debug compiler flags")
SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -g" CACHE INTERNAL "cxx debug compiler flags")
SET(CMAKE_ASM_FLAGS_DEBUG "-O0 -g" CACHE INTERNAL "asm debug compiler flags")

SET(CMAKE_C_FLAGS_RELEASE   "-Os -g" CACHE INTERNAL "c release compiler flags")
SET(CMAKE_CXX_FLAGS_RELEASE "-Os -g" CACHE INTERNAL "cxx release compiler flags")
SET(CMAKE_ASM_FLAGS_RELEASE "-Os -g" CACHE INTERNAL "asm release compiler flags")

# this makes the test compiles use static library option so that we don't need to pre-set linker flags and scripts
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
