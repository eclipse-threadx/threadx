# Toolchain settings
set(CMAKE_C_COMPILER    riscv32-unknown-elf-gcc)
set(CMAKE_CXX_COMPILER  riscv32-unknown-elf-g++)
set(AS                  riscv32-unknown-elf-as)
set(AR                  riscv32-unknown-elf-ar)
set(OBJCOPY             riscv32-unknown-elf-objcopy)
set(OBJDUMP             riscv32-unknown-elf-objdump)
set(SIZE                riscv32-unknown-elf-size)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# this makes the test compiles use static library option so that we don't need to pre-set linker flags and scripts
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_C_FLAGS   "${CFLAGS}" CACHE INTERNAL "c compiler flags")
set(CMAKE_CXX_FLAGS "${CXXFLAGS}" CACHE INTERNAL "cxx compiler flags")
if(DEFINED SOFT_FLOAT)
    set(CMAKE_ASM_FLAGS "${ASFLAGS} -D__ASSEMBLER__" CACHE INTERNAL "asm compiler flags")
else()
    set(CMAKE_ASM_FLAGS "${ASFLAGS} -D__ASSEMBLER__ -D__riscv_float_abi_single" CACHE INTERNAL "asm compiler flags")
endif()

set(CMAKE_EXE_LINKER_FLAGS "${LDFLAGS}" CACHE INTERNAL "exe link flags")

SET(CMAKE_C_FLAGS_DEBUG "-Og -g -ggdb3" CACHE INTERNAL "c debug compiler flags")
SET(CMAKE_CXX_FLAGS_DEBUG "-Og -g -ggdb3" CACHE INTERNAL "cxx debug compiler flags")
SET(CMAKE_ASM_FLAGS_DEBUG "-g -ggdb3" CACHE INTERNAL "asm debug compiler flags")

SET(CMAKE_C_FLAGS_RELEASE "-O3" CACHE INTERNAL "c release compiler flags")
SET(CMAKE_CXX_FLAGS_RELEASE "-O3" CACHE INTERNAL "cxx release compiler flags")
SET(CMAKE_ASM_FLAGS_RELEASE "" CACHE INTERNAL "asm release compiler flags")
