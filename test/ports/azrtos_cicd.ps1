<#
.SYNOPSIS
    Azure RTOS ThreadX CI/CD script

.DESCRIPTION
    Entry point for automated building, cleaning, testing, verification,  ...

.EXAMPLE
    .\azrtos_cicd.ps1 -clean
    This cleans all examples in the default database.

    .\azrtos_cicd.ps1 -MatchName 'Modules' -build
    This builds all ThreadX Modules examples.

    .\azrtos_cicd.ps1 -MatchName 'Modules','IAR' -build
    This builds all examples in the default database that have names matching the string 'Modules' and 'IAR'.

    .\azrtos_cicd.ps1 -MatchName 'Cortex M4','ARM compiler v6' -StartCLI
    This opens CLI with set environments for all examples in the default database that have names matching the 'Cortex M4' and 'ARM compiler v6'.

.LINK
    https://azure.com/rtos

.NOTES
    Author: Andres Mlinar
    Date:   2021
#>

[CmdletBinding(PositionalBinding=$false)] Param(

    [string[]]
    #One or more CSV files with the list of examples to process
    $CsvFile = "azrtos_cicd.csv",

    [string[]]
    #One or more regular expression strings to match names for
    $MatchName,

    [string[]]
    #One or more regular expression strings to match keywords for
    $MatchKeywords,

    [string[]]
    #One or more regular expression strings to match paths for
    $MatchPath,

    [string]
    #The source root directory, if not specified it defaults to the current repo
    $SrcDir = $( Join-Path $PSScriptRoot "..\.." ),

    [string]
    #The script root directory, if not specified it defaults to this script's directory
    $ScriptDir = $PSScriptRoot,

    [string]
    #The output log directory
    $LogDir = $( Join-Path $PSScriptRoot "log" ),

    [switch]
    #Start a CLI with a set environment for the specified examples
    $StartCLI = $false,

    [switch]
    #Check compliance for the specified examples
    $CheckCompliance = $false,

    [switch]
    #Clean all files not in the repository for the specified examples
    $RealClean = $false,

    [switch]
    #Clean the specified examples
    $Clean = $false,

    [switch]
    #Build the specified examples
    $Build = $false,

    [switch]
    #Test the specified examples
    $Test = $false

)

If ($PSVersionTable.PSVersion.Major -lt 7) {
    Write-Host "ERROR: PowerShell version 7 or higher is required!"
    Exit -2
}

Write-Verbose ("CSV files: " + $CsvFile)
Write-Verbose ("MatchName: " + $MatchName.Count + "[" + $MatchName + "]")
Write-Verbose ("MatchKeywords: " + $MatchKeywords.Count + "[" + $MatchKeywords + "]")
Write-Verbose ("MatchPath: " + $MatchPath.Count + "[" + $MatchPath + "]")
Write-Verbose ("SrcDir: " + $SrcDir)
Write-Verbose ("ScriptDir: " + $ScriptDir)
Write-Verbose ("LogDir: " + $LogDir)
Write-Verbose ("StartCLI? " + $StartCLI)
Write-Verbose ("CheckCompliance? " + $CheckCompliance)
Write-Verbose ("RealClean? " + $RealClean)
Write-Verbose ("Clean? " + $Clean)
Write-Verbose ("Build? " + $Build)
Write-Verbose ("Test? " + $Test)

$Stats = [PSCustomObject]@{
    Total               = 0
    CurrentCsv          = 0
    CleanScriptOK       = 0
    CleanScriptERROR    = 0
    BuildScriptOK       = 0
    BuildScriptERROR    = 0
    TestScriptOK        = 0
    TestScriptERROR     = 0
}

function Invoke-CustomScript {

    Param (

        [string]
        #A string describing the script type
        $Type,

        [string]
        #The sample name
        $Name,

        [string]
        #The environment setup script full path
        $EnvScript,

        [string]
        #The script full path
        $Script
    )

    Write-Verbose ("Type: " + $Type)
    Write-Verbose ("SampleDir: " + $SampleDir)
    Write-Verbose ("EnvScript: " + $EnvScript)
    Write-Verbose ("Script: " + $Script)

    If (-not (Test-Path -Path $EnvScript -PathType leaf)) {
        Write-Host "ERROR: the environment setup script doesn't exist:" $Script -ForegroundColor red
        return -1
    }

    If (-not (Test-Path -Path $Script -PathType leaf)) {
        Write-Host "ERROR: the script doesn't exist:" $Script -ForegroundColor red
        return -1
    }

    Write-Host ("`n`r")
    Write-Host (Get-Date -Format FileDateTimeUniversal) "INFO:" $Type "starting" "->" $Name -ForegroundColor green

    $ScriptOutput = & CMD /C "$EnvScript && $Script 2>&1"
    $ExitCode = $LASTEXITCODE
    Write-Verbose ("ExitCode: " + $ExitCode)
    Write-Verbose ("ScriptOutput: " + $ScriptOutput)

    $LogFileName = (Get-Date -Format FileDateTimeUniversal) + '.' + ($Name -replace ' ', '_' -replace '/', '_' -replace '\\', '_') + "." + $Type + "." + (($ExitCode -eq 0) ? "OK" : "ERROR") + ".log"
    $LogFileFullPath = Join-Path $LogDir $LogFileName
    Write-Verbose ("LogFileNmae: " + $LogFileName)
    Write-Verbose ("LogFileFullPath: " + $LogFileFullPath)

    $ScriptOutput | Out-File -FilePath $LogFileFullPath

    If ($ExitCode -ne 0) {
        Write-Host (Get-Date -Format FileDateTimeUniversal) "ERROR:" $Type "failed!" "->" $Name -ForegroundColor red
    } else {
        Write-Host (Get-Date -Format FileDateTimeUniversal) "INFO:" $Type "success!" "->" $Name -ForegroundColor green
    }

    return $ExitCode
}

# Create the log directory if it doesn't already exists
If (-Not (Test-Path -Path $LogDir -PathType Container)) {
    New-Item -Path $LogDir -ItemType Directory
}

ForEach ($f in $CsvFile) {

    Write-Host $(Get-Date -Format FileDateTimeUniversal) "INFO:" $f -ForegroundColor green

    Write-Verbose ("CSV file: " + $f)

    try {
        $t = Import-Csv -Path $f
    } catch {
        Write-Output "ERROR: failed to open CSV file:" $f -ForegroundColor red
        Continue
    }

    Write-Verbose ("Table rows: " + $t.Count)

    $Stats.CurrentCsv = 0

    ForEach ($i in $t) {

        $Stats.Total++
        $Stats.CurrentCsv++

        Write-Verbose ("`n`r")
        Write-Verbose ("Name: " + $i.Name)
        Write-Verbose ("Keywords: " + $i.Keywords)
        Write-Verbose ("Path: " + $i.Path)
        Write-Verbose ("HostPlatform: " + $i.HostPlatform)

        If ($i.HostPlatform -ne $PSVersionTable.Platform) {
            Write-Verbose ("HostPlatform mismatch, skipping...")
            Continue
        }

        $match = $true

        If ($MatchName.Count -ne 0) {
            Write-Verbose ("Matching name...")
            ForEach ($m in $MatchName) {
                Write-Verbose ("Matching " + $m)
                If (-not ($i.Name -match $m)) {
                    Write-Verbose ("Name mismatch: " + $m)
                    $match = $false;
                } 
            }
        }

        If ($MatchKeywords.Count -ne 0) {
            Write-Verbose ("Matching keywords...")
            ForEach ($m in $MatchKeywords) {
                Write-Verbose ("Matching " + $m)
                If (-not ($i.Keywords -match $m)) {
                    Write-Verbose ("Keywords mismatch: " + $m)
                    $match = $false 
                } 
            }
        }
        
        If ($MatchPath.Count -ne 0) {
            Write-Verbose ("Matching path...")
            ForEach ($m in $MatchPath) {
                Write-Verbose ("Matching " + $m)
                If (-not ($i.Path -match $m)) {
                    Write-Verbose ("Path mismatch: " + $m)
                    $match = $false
                } 
            }
        }

        If (-not $match) {
            Write-Verbose ("No match, skipping...")
            Continue
        }

        $FullPath = Join-Path $SrcDir $i.Path
        Write-Verbose ("FullPath: " + $FullPath)

        If (-Not $( Test-Path $FullPath -PathType Container )) {
            Write-Host "ERROR: path doesn't exist:" $FullPath -ForegroundColor red
            Continue
        }

        If ($StartCLI) {
            If ($PSVersionTable.Platform -eq "Win32NT") {
                Start-Process `
                    -FilePath "cmd.exe" `
                    -WorkingDirectory $FullPath `
                    -ArgumentList (
                        "/k " `
                        + ("set PATH=" + $PSScriptRoot + ";%PATH% & ") `
                        + ("DOSKEY realclean=" + "git clean -d -f -X " + $FullPath + " & ") `
                        + ("DOSKEY clean=" + (Join-Path $ScriptDir $i.CleanEnvScript) + '$T$T' + (Join-Path $ScriptDir $i.CleanScript) + " & ") `
                        + ("DOSKEY build=" + (Join-Path $ScriptDir $i.BuildEnvScript) + '$T$T' + (Join-Path $ScriptDir $i.BuildScript) + " & ") `
                        + ("DOSKEY test=" + (Join-Path $ScriptDir $i.TestEnvScript) + '$T$T' + (Join-Path $ScriptDir $i.TestScript))
                        )
            }
        }

        Push-Location $FullPath

        If ($RealClean ) {
            & ("git clean -d -f -X " + $FullPath)
        }

        If ($Clean -and ($i.CleanScript -ne "")) {
            Write-Progress -Activity $f -Status ($Stats.CurrentCsv.ToString() + "/" + $t.Count.ToString()) -CurrentOperation $i.Name -PercentComplete (1.0 * $Stats.CurrentCsv / $t.Count * 100) 
            $result = Invoke-CustomScript "Clean" $i.Name (Join-Path $ScriptDir $i.CleanEnvScript) (Join-Path $ScriptDir $i.CleanScript)
            if ($result -ne 0) {
                $Stats.CleanScriptERROR++
            } else {
                $Stats.CleanScriptOK++
            }
        }

        If ($Build -and ($i.BuildScript -ne "")) {
            Write-Progress -Activity $f -Status ($Stats.CurrentCsv.ToString() + "/" + $t.Count.ToString()) -CurrentOperation $i.Name -PercentComplete (1.0 * $Stats.CurrentCsv / $t.Count * 100) 
            $result = Invoke-CustomScript "Build" $i.Name (Join-Path $ScriptDir $i.BuildEnvScript) (Join-Path $ScriptDir $i.BuildScript)
            if ($result -ne 0) {
                $Stats.BuildScriptERROR++
            } else {
                $Stats.BuildScriptOK++
            }
        }

        If ($Test -and ($i.TestScript -ne "")) {
            Write-Progress -Activity $f -Status ($Stats.CurrentCsv.ToString() + "/" + $t.Count.ToString()) -CurrentOperation $i.Name -PercentComplete (1.0 * $Stats.CurrentCsv / $t.Count * 100) 
            $result = Invoke-CustomScript "Test" $i.Name (Join-Path $ScriptDir $i.TestEnvScript) (Join-Path $ScriptDir $i.TestScript)
            if ($result -ne 0) {
                $Stats.TestScriptERROR++
            } else {
                $Stats.TestScriptOK++
            }
        }

        Pop-Location
    }
}

"Total: " + $Stats.Total
If ($Clean) {
    "Clean: " + $Stats.CleanScriptOK + " OK, " + $Stats.CleanScriptERROR + " failed"
}
If ($Build) {
    "Build: " + $Stats.BuildScriptOK + " OK, " + $Stats.BuildScriptERROR + " failed"
}
If ($Test) {
    "Test: " + $Stats.TestScriptOK + " OK, " + $Stats.TestScriptERROR + " failed"
}

Exit $Stats.CleanScriptERROR+$Stats.BuildScriptERROR+$Stats.TestScriptERROR
