# Toolchain settings
set(CMAKE_C_COMPILER    clang-18)
set(CMAKE_CXX_COMPILER  clang++-18)
#set(AS                  llvm-as)
#set(AR                  llvm-ar)
#set(OBJCOPY             llvm-objcopy)
#set(OBJDUMP             llvm-objdump-18)
#set(SIZE                llvm-size)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# this makes the test compiles use static library option so that we don't need to pre-set linker flags and scripts
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_C_FLAGS   "${CFLAGS}" CACHE INTERNAL "c compiler flags")
set(CMAKE_CXX_FLAGS "${CXXFLAGS}" CACHE INTERNAL "cxx compiler flags")
set(CMAKE_ASM_FLAGS "${ASFLAGS} -D__ASSEMBLER__" CACHE INTERNAL "asm compiler flags")
set(CMAKE_EXE_LINKER_FLAGS "${LDFLAGS}" CACHE INTERNAL "exe link flags")

SET(CMAKE_C_FLAGS_DEBUG "--target=riscv32 -march=rv32im_zicsr_zicntr -mabi=ilp32 -g" CACHE INTERNAL "c debug compiler flags")
SET(CMAKE_CXX_FLAGS_DEBUG "--target=riscv32 -march=rv32im_zicsr_zicntr -mabi=ilp32 -g" CACHE INTERNAL "cxx debug compiler flags")
SET(CMAKE_ASM_FLAGS_DEBUG "--target=riscv32 -march=rv32im_zicsr_zicntr -mabi=ilp32 -g" CACHE INTERNAL "asm debug compiler flags")

SET(CMAKE_C_FLAGS_RELEASE "--target=riscv32 -march=rv32im_zicsr_zicntr -mabi=ilp32 -O3" CACHE INTERNAL "c release compiler flags")
SET(CMAKE_CXX_FLAGS_RELEASE "--target=riscv32 -march=rv32im_zicsr_zicntr -mabi=ilp32 -O3" CACHE INTERNAL "cxx release compiler flags")
SET(CMAKE_ASM_FLAGS_RELEASE "--target=riscv32 -march=rv32im_zicsr_zicntr -mabi=ilp32" CACHE INTERNAL "asm release compiler flags")
