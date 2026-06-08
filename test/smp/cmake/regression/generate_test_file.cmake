if(NOT DEFINED SOURCE_FILE)
    message(FATAL_ERROR "SOURCE_FILE is required")
endif()

if(NOT DEFINED OUTPUT_FILE)
    message(FATAL_ERROR "OUTPUT_FILE is required")
endif()

file(STRINGS "${SOURCE_FILE}" FILE_LINES)

set(UPDATED_FILE_CONTENTS "")
set(DISPATCH_DECLARATION "VOID                            test_interrupt_dispatch(VOID);")
set(DISPATCH_CALL "test_interrupt_dispatch();")
set(DECLARATION_INSERTED FALSE)
set(CALL_INSERTED FALSE)

foreach(FILE_LINE IN LISTS FILE_LINES)
    if((NOT DECLARATION_INSERTED) AND
       ((FILE_LINE MATCHES "^void[ \t]+\\*_tx_linux_timer_interrupt\\(void \\*p\\);[ \t]*$")
        OR
        (FILE_LINE MATCHES "^VOID[ \t]+_tx_win32_timer_interrupt\\(VOID\\);[ \t]*$")
        OR
        (FILE_LINE MATCHES "^VOID CALLBACK[ \t]+_tx_win32_timer_interrupt\\(UINT wTimerID, UINT msg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2\\);[ \t]*$")))
        string(APPEND UPDATED_FILE_CONTENTS "${FILE_LINE}\n${DISPATCH_DECLARATION}\n")
        set(DECLARATION_INSERTED TRUE)
    elseif((NOT CALL_INSERTED) AND (FILE_LINE MATCHES "^([ \t]*)_tx_timer_interrupt\\(\\);[ \t]*$"))
        string(APPEND UPDATED_FILE_CONTENTS "${CMAKE_MATCH_1}${DISPATCH_CALL}\n${FILE_LINE}\n")
        set(CALL_INSERTED TRUE)
    else()
        string(APPEND UPDATED_FILE_CONTENTS "${FILE_LINE}\n")
    endif()
endforeach()

if(NOT CALL_INSERTED)
    message(FATAL_ERROR "Unable to insert test interrupt dispatcher call into ${SOURCE_FILE}")
endif()

if(NOT DECLARATION_INSERTED)
    message(FATAL_ERROR "Unable to insert test interrupt dispatcher declaration into ${SOURCE_FILE}")
endif()

get_filename_component(OUTPUT_DIRECTORY "${OUTPUT_FILE}" DIRECTORY)
file(MAKE_DIRECTORY "${OUTPUT_DIRECTORY}")
file(WRITE "${OUTPUT_FILE}" "${UPDATED_FILE_CONTENTS}")
