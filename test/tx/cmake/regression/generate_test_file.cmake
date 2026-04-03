if(NOT DEFINED SOURCE_FILE)
    message(FATAL_ERROR "SOURCE_FILE is required")
endif()

if(NOT DEFINED OUTPUT_FILE)
    message(FATAL_ERROR "OUTPUT_FILE is required")
endif()

file(READ "${SOURCE_FILE}" FILE_CONTENTS)

set(TIMER_CALL_BLOCK
"    /* Call the ThreadX system timer interrupt processing.  */\n    _tx_timer_interrupt();")
set(TIMER_CALL_BLOCK_REPLACEMENT
"    test_interrupt_dispatch();\n\n    /* Call the ThreadX system timer interrupt processing.  */\n    _tx_timer_interrupt();")

string(REPLACE "${TIMER_CALL_BLOCK}" "${TIMER_CALL_BLOCK_REPLACEMENT}" UPDATED_FILE_CONTENTS "${FILE_CONTENTS}")

if(UPDATED_FILE_CONTENTS STREQUAL FILE_CONTENTS)
    message(FATAL_ERROR "Unable to insert test interrupt dispatcher call into ${SOURCE_FILE}")
endif()

set(FILE_CONTENTS "${UPDATED_FILE_CONTENTS}")

set(LINUX_DECLARATION "void               *_tx_linux_timer_interrupt(void *p);")
set(WINDOWS_DECLARATION "VOID CALLBACK                   _tx_win32_timer_interrupt(UINT wTimerID, UINT msg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2);")
set(DISPATCH_DECLARATION "VOID                            test_interrupt_dispatch(VOID);")

if(FILE_CONTENTS MATCHES "_tx_linux_timer_interrupt")
    string(REPLACE "${LINUX_DECLARATION}" "${LINUX_DECLARATION}\n${DISPATCH_DECLARATION}" UPDATED_FILE_CONTENTS "${FILE_CONTENTS}")
elseif(FILE_CONTENTS MATCHES "_tx_win32_timer_interrupt")
    string(REPLACE "${WINDOWS_DECLARATION}" "${WINDOWS_DECLARATION}\n${DISPATCH_DECLARATION}" UPDATED_FILE_CONTENTS "${FILE_CONTENTS}")
else()
    message(FATAL_ERROR "Unsupported timer interrupt source file: ${SOURCE_FILE}")
endif()

if(UPDATED_FILE_CONTENTS STREQUAL FILE_CONTENTS)
    message(FATAL_ERROR "Unable to insert test interrupt dispatcher declaration into ${SOURCE_FILE}")
endif()

get_filename_component(OUTPUT_DIRECTORY "${OUTPUT_FILE}" DIRECTORY)
file(MAKE_DIRECTORY "${OUTPUT_DIRECTORY}")
file(WRITE "${OUTPUT_FILE}" "${UPDATED_FILE_CONTENTS}")
