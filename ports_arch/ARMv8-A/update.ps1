<#
.SYNOPSIS
    Update the ARMv8-A ports from the generic ARMv8-A port.

.DESCRIPTION
    Generate ports, examples and tests from common files.

.EXAMPLE
    To update the ThreadX ports for the source tree run:
    pwsh -Command .\update.ps1 -PortSets tx -CopyCommonFiles -CopyPortFiles -CopyExample -PatchFiles

    To update the ThreadX ports for the test tree run:
    pwsh -Command .\update.ps1 -PortSets tx -CopyCommonFiles -CopyPortFiles -CopyExample -CopyValidationTest -CopyRegressionTest -PatchFiles

.LINK
    https://azure.com/rtos

.NOTES
    Author: Andres Mlinar
    Date:   2021
#>

[CmdletBinding(PositionalBinding=$false)] Param(

    [string]
    #The script root directory, if not specified it defaults to this script's directory
    $ScriptDir = $PSScriptRoot,

    [Parameter(Mandatory)]
    [ValidateSet(
        "tx", "tx_smp",
        "txm", "txm_smp"
    )]
    [string[]]
    #Specify the flavor or flavors of ThreadX to generate. Options are: tx, tx_smp, txm, txm_smp
    $PortSets,

    [switch]
    #Copy common files
    $CopyCommonFiles = $false,

    [switch]
    #Copy port files
    $CopyPortFiles = $false,

    [switch]
    #Copy the example
    $CopyExample = $false,

    [switch]
    #Copy the validation tests
    $CopyValidationTest = $false,

    [switch]
    #Copy the regression tests
    $CopyRegressionTest = $false,

    [switch]
    #Copy port files
    $PatchFiles = $false,

    [string]
    #The output log directory
    $LogDir = $( Join-Path $PSScriptRoot 'log' )

)

Write-Host "Update the ARMv8 ports"

Write-Verbose ("Script directory: $ScriptDir")
Write-Verbose ("Port sets: $PortSets")
Write-Verbose ("Copy common files: $CopyCommonFiles")
Write-Verbose ("Copy port files: $CopyPortFiles")
Write-Verbose ("Copy example: $CopyExample")
Write-Verbose ("Copy validation test: $CopyValidationTest")
Write-Verbose ("Copy regression test: $CopyRegressionTest")
Write-Verbose ("Patch files: $PatchFiles")
Write-Verbose ("LogDir: $LogDir")

$cores = @("cortex_a35", "cortex_a53", "cortex_a55", "cortex_a57", "cortex_a65", "cortex_a65ae", "cortex_a72", "cortex_a73", "cortex_a75", "cortex_a76", "cortex_a76ae", "cortex_a77")
$compilers = @("ac6", "gnu", "iar", "ghs")
$patches = (
    ('example_build\sample_threadx\.cproject', (
        (('value="cortex-a35"'), ('value="cortex-$core_short_lower"')),
        (('Cortex-A35.AArch64.ARMv8.Neon.Crypto'), ('Cortex-$($core_short_upper).AArch64.ARMv8.Neon.Crypto'))
    )),
    ('example_build\tx\.cproject', (
        (('value="cortex-a35"'), ('value="cortex-$($core_short_lower)"')),
        (('Cortex-A35.AArch64.ARMv8.Neon.Crypto'), ('Cortex-$($core_short_upper).AArch64.ARMv8.Neon.Crypto'))
    )),
    ('example_build\sample_threadx\sample_threadx.launch', (
        ('Debug Cortex-A35', 'Debug Cortex-$($core_short_upper)'),
        ('Base_A35x1', 'Base_$($core_short_upper)x1'),
        ('base_A35x1', 'base_$($core_short_lower)x1'),
        ('FVP_Base_Cortex-A35x1', 'FVP_Base_Cortex-$($core_short_upper)x1'),
        ('Base_A35x2', 'Base_$($core_short_upper)x2'),
        ('base_A35x2', 'base_$($core_short_lower)x2'),
        ('FVP_Base_Cortex-A35x2', 'FVP_Base_Cortex-$($core_short_upper)x2'),
        ('Base_A35x4', 'Base_$($core_short_upper)x4'),
        ('base_A35x4', 'base_$($core_short_lower)x4'),
        ('FVP_Base_Cortex-A35x4', 'FVP_Base_Cortex-$($core_short_upper)x4')
    ))
)

# Create the log directory if it doesn't already exists
If (-Not (Test-Path -Path $LogDir -PathType Container)) {
    $LogDirObject = New-Item -Path $LogDir -ItemType Directory
}

Function Copy-FilesVerbose {
    [CmdletBinding()] Param (
        [string] $source, 
        [string] $destination_directory
    )
    Write-Verbose ("Copying common files...")
    Write-Verbose ("Copy: " + $source + " -> " + $destination_directory)
    Copy-Item -Path $source -Destination $destination_directory -Recurse -Force
    Write-Verbose("Done.")
}

ForEach ($PortSet in $PortSets) {
    ForEach ($core in $cores) {
        Switch ($PortSet) {
            "tx" { $core_directory = "..\..\ports\" + $core }
            "tx_smp" { $core_directory = "..\..\ports_smp\" + $core + "_smp" }
            "txm" { $core_directory = "..\..\ports_module\" + $core }
            "txm_smp" { $core_directory = "..\..\ports_module_smp\" + $core + "_smp" }
            Default {}
        }
        ForEach ($compiler in $compilers) {
            $compiler_directory = $core_directory + "\" + $compiler
            Write-Verbose ("Compiler directory: $compiler_directory")
            $compiler_directory_object = New-Item -Path $compiler_directory -ItemType "directory" -Force
    
            $destination_directory = $compiler_directory
    
            If ($CopyCommonFiles) {
                Copy-FilesVerbose -source "threadx\common\*" -destination_directory $destination_directory
            }

            If ($CopyPortFiles) {
                Copy-FilesVerbose -source "threadx\ports\$compiler\*" -destination_directory $destination_directory
            }
    
            If ($PortSet -eq 'tx_smp') {
                If ($CopyCommonFiles) {
                    Copy-FilesVerbose -source "threadx_smp\common\*" -destination_directory $destination_directory
                }
                If ($CopyPortFiles) {
                    Copy-FilesVerbose -source "threadx_smp\ports\$compiler\*" -destination_directory $destination_directory
                }
            }

            If ($PortSet -eq 'txm') {
            }

            If ($PortSet -eq 'txm_smp') {
            }

            If ($PatchFiles) {
                ForEach ($patch in $patches) {
                    $core_short = $core -Replace "cortex_",""
                    $core_short_upper = $core_short.ToUpper()
                    $core_short_lower = $core_short.ToLower()
                    ForEach ($patch in $patches) {
                        $path = $destination_directory + "\" + $patch[0]
                        Write-Verbose("Patching file: $path")
                        If (Test-Path -Path $path -PathType leaf) {
                            $content = Get-Content -Path $path
                            ForEach ($replacement in $patch[1]) {
                                $original = $replacement[0]
                                $substitute = $replacement[1]
                                $original = $ExecutionContext.InvokeCommand.ExpandString($original)
                                $substitute = $ExecutionContext.InvokeCommand.ExpandString($substitute)
                                Write-Verbose("`tpatch: `"$original`" -> `"$substitute`"")
                                $content = $content -creplace $original,$substitute
                            }
                            Set-Content -Path $path -Value $content -Encoding ascii
                            Write-Verbose("Patched.")
                        } Else {
                            Write-Verbose("File not found.")
                        }
                    }
                }
            }
        }
    }
}
