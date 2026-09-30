cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

project(demo_threadx_erbium LANGUAGES C ASM)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(ERBIUM_DIR   ${CMAKE_CURRENT_LIST_DIR})
set(THREADX_ROOT ${CMAKE_CURRENT_LIST_DIR}/../../../../..)

add_subdirectory(${THREADX_ROOT} threadx)

add_executable(demo_threadx
    ${ERBIUM_DIR}/entry.S
    ${ERBIUM_DIR}/tx_initialize_low_level.S
    ${ERBIUM_DIR}/board.c
    ${ERBIUM_DIR}/hwtimer.c
    ${ERBIUM_DIR}/plic.c
    ${ERBIUM_DIR}/trap.c
    ${ERBIUM_DIR}/uart.c
    ${ERBIUM_DIR}/demo_threadx.c
)
set_target_properties(demo_threadx PROPERTIES
    OUTPUT_NAME "demo_threadx.elf"
    LINK_DEPENDS ${ERBIUM_DIR}/link.lds
)

target_include_directories(demo_threadx PRIVATE ${ERBIUM_DIR})

target_link_libraries(demo_threadx PRIVATE threadx)

target_compile_options(demo_threadx PRIVATE
    -Wall
    -ffreestanding
    -ffunction-sections
    -fdata-sections
)

target_link_options(demo_threadx PRIVATE
    -T${ERBIUM_DIR}/link.lds
    -nostdlib
    -Wl,--gc-sections
    -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/demo_threadx.map
)

function(add_erbium_test name source handler)
    add_executable(${name}
        ${ERBIUM_DIR}/entry.S
        ${ERBIUM_DIR}/tx_initialize_low_level.S
        ${ERBIUM_DIR}/board.c
        ${ERBIUM_DIR}/hwtimer.c
        ${ERBIUM_DIR}/plic.c
        ${ERBIUM_DIR}/uart.c
        ${source}
        ${handler}
    )
    target_include_directories(${name} PRIVATE ${ERBIUM_DIR})
    target_link_libraries(${name} PRIVATE threadx)
    target_compile_options(${name} PRIVATE -ffreestanding)
    target_link_options(${name} PRIVATE
        -T${ERBIUM_DIR}/link.lds
        -nostdlib
        -Wl,--gc-sections
    )
    set_target_properties(${name} PROPERTIES LINK_DEPENDS ${ERBIUM_DIR}/link.lds)
endfunction()

add_erbium_test(test_uart_ticks ${ERBIUM_DIR}/test/test_uart_ticks.c ${ERBIUM_DIR}/trap.c)
add_erbium_test(test_plic_race ${ERBIUM_DIR}/test/test_plic_race.c ${ERBIUM_DIR}/test/test_plic_trap.c)
