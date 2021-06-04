
call build_threadx.bat
call build_threadx_module_library.bat
call build_threadx_module_sample.bat
call ..\..\..\..\common_modules\utilities\module_to_c_array.exe sample_threadx_module.axf module_code.c
powershell -Command "(gc module_code.c) -replace 'unsigned', '__align(4096) unsigned' | Out-File module_code.c"
call build_threadx_module_manager_sample.bat
