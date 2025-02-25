# Build sample_threadx program

project(sample_threadx)
set(SRCS sample_threadx.c)

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE ${SRCS})
set_target_properties(
  ${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                             ${CMAKE_CURRENT_BINARY_DIR}/../../../../)
target_include_directories(
  ${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../inc
                          ${CMAKE_CURRENT_LIST_DIR}/../../../../common${THREADX_SMP}/inc)

target_link_libraries(${PROJECT_NAME} PRIVATE threadx)
