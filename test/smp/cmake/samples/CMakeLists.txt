cmake_minimum_required(VERSION 3.13 FATAL_ERROR)
cmake_policy(SET CMP0057 NEW)

project(samples LANGUAGES C)

set(SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/../../../../ports_smp/linux/gnu/example_build)

set(sample_files
    ${SOURCE_DIR}/sample_threadx.c)

foreach(sample_file ${sample_files})
  get_filename_component(sample_file_name ${sample_file} NAME_WE)
  add_executable(${sample_file_name} ${sample_file} ${CMAKE_CURRENT_LIST_DIR}/fake.c)
  target_link_libraries(${sample_file_name} PRIVATE azrtos::threadx_smp)
endforeach()
