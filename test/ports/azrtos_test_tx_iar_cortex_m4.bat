@REM
@REM You can launch cspybat by typing the name of this batch file followed
@REM by the name of the debug file (usually an ELF/DWARF or UBROF file).
@REM
@REM Read about available command line parameters in the C-SPY Debugging
@REM Guide. Hints about additional command line parameters that may be
@REM useful in specific cases:
@REM   --download_only   Downloads a code image without starting a debug
@REM                     session afterwards.
@REM   --silent          Omits the sign-on message.
@REM   --timeout         Limits the maximum allowed execution time.
@REM 


@echo off 

if not "%~1" == "" goto debugFile 

@echo on 

@REM "C:\Program Files (x86)\IAR Systems\Embedded Workbench 8.4\common\bin\cspybat" -f "C:\andres\git\AzureRTOS\threadx\ports\cortex_m4\iar\example_build\settings\sample_threadx.Debug.general.xcl" --macro unit_test.mac --backend -f "C:\andres\git\AzureRTOS\threadx\ports\cortex_m4\iar\example_build\settings\sample_threadx.Debug.driver.xcl"  
"C:\Program Files (x86)\IAR Systems\Embedded Workbench 8.4\common\bin\cspybat" -f "C:\andres\git\AzureRTOS\threadx\ports\cortex_m4\iar\example_build\settings\sample_threadx.Debug.general.xcl" --backend -f "C:\andres\git\AzureRTOS\threadx\ports\cortex_m4\iar\example_build\settings\sample_threadx.Debug.driver.xcl"  

@echo off 
goto end 

:end