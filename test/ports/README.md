# Azure RTOS ThreadX CI/CD system

**The old Azure RTOS ThreadX CI/CD script and README are preserved with the .old extension**

This directory contains a collection of files to execute test for continuos integration and continuous delivery of Azure RTOS ThreadX.

This directory contains documentation, scripts and auxiliary files (expected test results, etc).

The Azure RTOS ThreadX CI/CD system can be used in various scenarios:
* Setting up the environment for a toolchain
* Cleaning up the local repository
* Building locally or on the pipeline
* Smoke testing examples (on the native IDE)
* Testing an example on a generic architecture simulator (like ARM FVP)
* Running the certification test suite on a port
* Testing locally during development
* Testing automatically on the DevOps pipeline
* Testing ports
* Testing examples
* Testing one or more ports
* Verifying the ports build
* Verifying that examples one or more ports during development

There are several operations that can be performed by the CI/CD system:
* CheckCompliance: perform compliance checks (only debug configurations, valid comments, ...)
* StartCLI: start a CLI environment.
* RealClean: remove all generated files.
* Clean: remove all build generated files.
* Build: compile examples using using the toolchain IDE, project, workspace, etc.
* Test: smoke test examples with the toolchain simulator.
* PlatformTest: test examples using the platform simulator, like the ARM FVP platform.
* CertificationTest: test examples using the certification test suite.
* HardwareTest: test examples using attached hardware.

## azrtos_cicd.ps1
This is the main entry point for Azure RTOS ThreadX CI/CD operations.
To get detailed usage information use the PowerShell Get-Help command passing this script as a parameter.
This file outputs a summary to the screen and detailed log file with all output to files located in the default log directory or a directory specified in the script command line.

## azrtos_cicd.csv
This is the default database of ports, examples and corresponding data and scripts. The database is a CSV file that can be modified or updated by the user. Other databases can be specified on the command line when calling the script.

## -StartCLI
This option will open a CLI with the environment appropriately set for it. It will have the command aliases "realclean", "clean", "build" and "test" set to the corresponding scripts. Useful for developing and debugging.

## Examples:

These examples assume the PowerShell script is called from the Windows Command Interpreter. For calling the script from the PowerShell prompt, remove the leading `pwsh -Command`.

`pwsh -Command Get-Help .\azrtos_cicd.ps1`
Show the help embedded in the script.

`pwsh -Command .\azrtos_cicd.ps1 -clean`
This cleans all examples in the default database.

`pwsh -Command .\azrtos_cicd.ps1 -build`
This builds all examples in the default database.

`pwsh -Command .\azrtos_cicd.ps1 -test`
This runs the bassic tests on all examples in the default database.

`pwsh -Command .\azrtos_cicd.ps1 -clean -build -test`
This first cleans, then builds and finally runs tests on all examples in the default database.

`pwsh -Command .\azrtos_cicd.ps1 -MatchName 'Modules' -build`
This builds all ThreadX Modules examples.

`pwsh -Command .\azrtos_cicd.ps1 -MatchName 'Modules','IAR' -build`
This builds all examples in the default database that have names matching the string 'Modules' and 'IAR'.

`pwsh -Command .\azrtos_cicd.ps1 -MatchName 'Cortex M4','ARM compiler v6' -StartCli`
This opens a CLI with set environment for all examples in the default database that have names matching the 'Cortex M4' and 'ARM compiler v6'.

`pwsh -Command .\azrtos_cicd.ps1 -MatchKeywords 'TX ','ARM compiler v6','Cortex M' -clean`
This cleans all examples matching all the keywords; these are ThreadX (not ThreadX modules) Cortex M (0/3/4/7/23/33/...) for ARM compiler v6.

`pwsh -Command .\azrtos_cicd.ps1 -MatchKeywords 'TX ','ARM compiler v6','Cortex M' -build`
Like the example above, but it builds the matching examples.

`pwsh -Command .\azrtos_cicd.ps1 -MatchKeywords 'TX ','Cortex M7' -StartCLI`
Start CLI environments for all ThreadX for Cortex M7 examples (IAR, ARM compiler v5, ARM compiler v6, GHS, ...).
From these you can use the commands 'realclean','clean','build','test' to call these operations and see the output on the console.

Here is a list of commonly used commands that you can directly cut/paste into your command line:
`pwsh -Command Get-Help .\azrtos_cicd.ps1`
`pwsh -Command .\azrtos_cicd.ps1 -clean`
`pwsh -Command .\azrtos_cicd.ps1 -build`
`pwsh -Command .\azrtos_cicd.ps1 -test`
`pwsh -Command .\azrtos_cicd.ps1 -clean -build -test`
`pwsh -Command .\azrtos_cicd.ps1 -MatchKeywords 'TX ','ARM compiler v6','Cortex M' -clean`
`pwsh -Command .\azrtos_cicd.ps1 -MatchKeywords 'TX ','IAR','Cortex M' -clean`
`pwsh -Command .\azrtos_cicd.ps1 -MatchKeywords 'TX ','GCC','Cortex M' -clean`
`pwsh -Command .\azrtos_cicd.ps1 -MatchKeywords 'TX ','ARM compiler v6','Cortex M' -build`
`pwsh -Command .\azrtos_cicd.ps1 -MatchKeywords 'TX ','IAR','Cortex M' -build`
`pwsh -Command .\azrtos_cicd.ps1 -MatchKeywords 'TX ','GCC','Cortex M' -build`
`pwsh -Command .\azrtos_cicd.ps1 -MatchKeywords 'TXM','ARM compiler v6','Cortex M' -clean`
`pwsh -Command .\azrtos_cicd.ps1 -MatchKeywords 'TXM','IAR','Cortex M' -clean`
`pwsh -Command .\azrtos_cicd.ps1 -MatchKeywords 'TXM','GCC','Cortex M' -clean`
`pwsh -Command .\azrtos_cicd.ps1 -MatchKeywords 'TXM','ARM compiler v6','Cortex M' -build`
`pwsh -Command .\azrtos_cicd.ps1 -MatchKeywords 'TXM','IAR','Cortex M' -build`
`pwsh -Command .\azrtos_cicd.ps1 -MatchKeywords 'TXM','GCC','Cortex M' -build`
`pwsh -Command .\azrtos_cicd.ps1 -MatchKeywords 'TX ','ARM compiler v6','SMP' -clean`
`pwsh -Command .\azrtos_cicd.ps1 -MatchKeywords 'TX ','IAR','SMP' -clean`
`pwsh -Command .\azrtos_cicd.ps1 -MatchKeywords 'TX ','GCC','SMP' -clean`
`pwsh -Command .\azrtos_cicd.ps1 -MatchKeywords 'TX ','ARM compiler v6','SMP' -build`
`pwsh -Command .\azrtos_cicd.ps1 -MatchKeywords 'TX ','IAR','SMP' -build`
`pwsh -Command .\azrtos_cicd.ps1 -MatchKeywords 'TX ','GCC','SMP' -build`
`pwsh -Command .\azrtos_cicd.ps1 -MatchKeywords 'TX ','ARM compiler v6','ARMv8-A' -build`
`pwsh -Command .\azrtos_cicd.ps1 -MatchKeywords 'TX ','GCC','ARMv8-A' -build`
`pwsh -Command .\azrtos_cicd.ps1 -MatchKeywords 'TX ','ARM compiler v6','Cortex-A35' -clean`
`pwsh -Command .\azrtos_cicd.ps1 -MatchKeywords 'TX ','GCC','Cortex-A35' -clean`
`pwsh -Command .\azrtos_cicd.ps1 -MatchKeywords 'TX ','ARM compiler v6','Cortex-A35' -build`
`pwsh -Command .\azrtos_cicd.ps1 -MatchKeywords 'TX ','GCC','Cortex-A35' -build`
`pwsh -Command .\azrtos_cicd.ps1 -MatchKeywords 'TX ','ARM compiler v6','Cortex-A35' -test`
`pwsh -Command .\azrtos_cicd.ps1 -MatchKeywords 'TX ','GCC','Cortex-A35' -test`
`pwsh -Command .\azrtos_cicd.ps1 -MatchKeywords 'ARM compiler v6','Cortex-A35' -clean -build -test`
`pwsh -Command .\azrtos_cicd.ps1 -MatchKeywords 'GCC','Cortex-A35' -clean -build -test`

For more examples look at the help embedded in the script.
