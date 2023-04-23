# Azure RTOS CI/CD (old version)

**There is a new version of this script written in PowerSell and now named azrtos_cicd.ps1. Use that script instead. The documentation for that script is in the file README.md and the embedded help of that script.**

This directory contains a collection of files to execute several automated operations on all ports.
These are batch scripts and auxiliary files (expected test results, etc).

## These operations include:
* Build: compile the port example.
* Clean: remove all build generated files.
* Test: test the example with the toolchain simulator.
* Test_FVP: test the example using the ARM FVP platform.

## azrtos_cicd.old.bat
This is the main entry point for operations to be performed on several examples at the same time.  
This script can be called from any location, it will automatically perform the requested operations on the examples of the repository where it is located.  
This file outputs a summary to the screen and a detailed log file with all the output called azrtos_do.all.log  
Call this scripts without parameters to get usage information.  
The script parameters are case insensitive.  

## Examples:

`azrtos_cicd.old.bat build tx ghs iar`  
This will build all GHS and IAR example projects.

`azrtos_cicd.old.bat clean tx all`  
This cleans all ThreadX examples

`azrtos_cicd.old.bat build txm iar`  
This builds all ThreadX Modules IAR examples.
