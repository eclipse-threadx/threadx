target_sources(${PROJECT_NAME}
    PRIVATE
    # {{BEGIN_TARGET_SOURCES}}
    ${CMAKE_CURRENT_LIST_DIR}/src/tx_thread_context_restore.c
    ${CMAKE_CURRENT_LIST_DIR}/src/tx_thread_context_save.c
    ${CMAKE_CURRENT_LIST_DIR}/src/tx_thread_interrupt_control.c
    ${CMAKE_CURRENT_LIST_DIR}/src/tx_thread_schedule.c
    ${CMAKE_CURRENT_LIST_DIR}/src/tx_thread_stack_build.c
    ${CMAKE_CURRENT_LIST_DIR}/src/tx_thread_system_return.c
    ${CMAKE_CURRENT_LIST_DIR}/src/tx_timer_interrupt.c
    # {{END_TARGET_SOURCES}}
)

target_include_directories(${PROJECT_NAME}
    SYSTEM
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/inc
)

# The port includes Infineon iLLD headers (Compilers.h, Bsp.h, IfxCpu.h,
# IfxStm.h). AURIX_ILLD_DIR shall name the directory that holds the iLLD, Infra
# and Service folders: either the iLLD release package for all TC3xx devices
# (iLLD/TC3xx, for example src/BaseSw of iLLD 1.22.0) or the TC37A iLLD of an
# AURIX Development Studio project (iLLD/TC37A). The iLLD in turn includes the
# board configuration Ifx_Cfg.h from AURIX_CFG_DIR, which defaults to the
# TC375 Lite Kit configuration in example_build/aurix_lite_kit_v2.
if(NOT AURIX_ILLD_DIR)
    message(FATAL_ERROR "AURIX_ILLD_DIR not defined")
endif()
set(AURIX_CFG_DIR "${CMAKE_CURRENT_LIST_DIR}/example_build/aurix_lite_kit_v2" CACHE PATH
    "Directory that holds the board's Ifx_Cfg.h")

if(IS_DIRECTORY "${AURIX_ILLD_DIR}/iLLD/TC3xx/Tricore")
    set(AURIX_ILLD "${AURIX_ILLD_DIR}/iLLD/TC3xx/Tricore")
    set(AURIX_SFR "${AURIX_ILLD_DIR}/Infra/Sfr/TC37x")
    set(AURIX_PINMAP "${AURIX_ILLD}/_PinMap/TC37x")
elseif(IS_DIRECTORY "${AURIX_ILLD_DIR}/iLLD/TC37A/Tricore")
    set(AURIX_ILLD "${AURIX_ILLD_DIR}/iLLD/TC37A/Tricore")
    set(AURIX_SFR "${AURIX_ILLD_DIR}/Infra/Sfr/TC37A/_Reg")
    set(AURIX_PINMAP "${AURIX_ILLD}/_PinMap")
else()
    message(FATAL_ERROR "AURIX_ILLD_DIR contains neither iLLD/TC3xx/Tricore nor iLLD/TC37A/Tricore")
endif()

target_include_directories(${PROJECT_NAME}
    PUBLIC
    ${AURIX_CFG_DIR}
    ${AURIX_ILLD}
    ${AURIX_ILLD}/Cpu/Std
    ${AURIX_ILLD}/Cpu/Irq
    ${AURIX_ILLD}/Cpu/Trap
    ${AURIX_ILLD}/Scu/Std
    ${AURIX_ILLD}/Src/Std
    ${AURIX_ILLD}/Stm/Std
    ${AURIX_ILLD}/Port/Std
    ${AURIX_ILLD}/_Impl
    ${AURIX_ILLD}/_PinMap
    ${AURIX_PINMAP}
    ${AURIX_ILLD_DIR}/Infra/Platform
    ${AURIX_ILLD_DIR}/Infra/Platform/Tricore/Compilers
    ${AURIX_SFR}
    ${AURIX_ILLD_DIR}/Service/CpuGeneric
    ${AURIX_ILLD_DIR}/Service/CpuGeneric/SysSe/Bsp
    ${AURIX_ILLD_DIR}/Service/CpuGeneric/_Utilities
)
