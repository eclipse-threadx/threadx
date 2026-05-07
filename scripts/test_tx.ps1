[CmdletBinding()]
param(
    [ValidateSet('win64', 'win32')]
    [string]$Arch = 'win64',

    [AllowNull()]
    [object]$Configuration = 'all',

    [int]$Parallel = [Math]::Max(1, [Environment]::ProcessorCount),

    [int]$RepeatFailCount = 1,

    [int]$TestTimeoutSeconds = 20,

    [switch]$CollectFailureDiagnostics = $true,

    [string]$TestRegex,

    [switch]$RerunFailedOnly,

    [string]$BuildDir,

    [switch]$Clean
)

$ErrorActionPreference = 'Stop'
. (Join-Path $PSScriptRoot 'tx_windows_common.ps1')

$repoRoot = Split-Path -Parent $PSScriptRoot
$settings = Get-PortSettings -SelectedArch $Arch

if (-not $BuildDir) {
    $BuildDir = Join-Path $repoRoot "build\tests\$Arch"
}

$selectedConfigurations = Resolve-RegressionConfigurations -RequestedConfigurations $Configuration
Write-Host "Selected configurations: $($selectedConfigurations -join ', ')"

Enter-VisualStudioDevShell -VsArch $settings.VsArch

if (($settings.ThreadXArch -eq 'win32') -or ($settings.ThreadXArch -eq 'win64')) {
    if ($Parallel -ne 1) {
        Write-Warning "Windows simulator regression tests are timing-sensitive under concurrent ctest execution. Forcing -Parallel 1."
        $Parallel = 1
    }
}

$failedConfigurations = @()

foreach ($currentConfiguration in $selectedConfigurations) {
    $currentBuildDirName = Get-RegressionBuildDirectoryName -ConfigurationName $currentConfiguration
    $currentBuildDir = Join-Path $BuildDir $currentBuildDirName
    $currentTestingTemporaryDir = Join-Path $currentBuildDir 'Testing\Temporary'

    try {
        if ($Clean) {
            $currentTestingDir = Join-Path $currentBuildDir 'Testing'
            Remove-CtestTestingDirectory -Path $currentTestingDir
        }

        if (-not (Test-Path -LiteralPath $currentBuildDir)) {
            throw "Build directory does not exist for $Arch / ${currentConfiguration}: $currentBuildDir"
        }

        Remove-NinjaLock -Path $currentBuildDir
        if (Test-Path -LiteralPath $currentTestingTemporaryDir) {
            Remove-Item -LiteralPath (Join-Path $currentTestingTemporaryDir 'LastTest.log') -Force -ErrorAction SilentlyContinue
            Remove-Item -LiteralPath (Join-Path $currentTestingTemporaryDir 'LastTestsFailed.log') -Force -ErrorAction SilentlyContinue
        }

        Write-Host "Testing $Arch / $currentConfiguration"
        $ctestArguments = @(
            '--test-dir', $currentBuildDir,
            '--output-on-failure',
            '--timeout', $TestTimeoutSeconds.ToString(),
            '-j', $Parallel.ToString()
        )

        if ($RepeatFailCount -gt 1) {
            $ctestArguments += @('--repeat', "until-pass:$RepeatFailCount")
        }

        if ($TestRegex) {
            $ctestArguments += @('-R', $TestRegex)
        }

        if ($RerunFailedOnly) {
            $ctestArguments += '--rerun-failed'
        }

        Invoke-NativeCommand -FilePath 'ctest' -Arguments $ctestArguments
    }
    catch {
        if ($CollectFailureDiagnostics -and (Test-Path -LiteralPath $currentBuildDir)) {
            try {
                Invoke-CtestFailureDiagnostics -BuildDir $currentBuildDir -TestingTemporaryDir $currentTestingTemporaryDir `
                    -TimeoutSeconds $TestTimeoutSeconds
            }
            catch {
                Write-Warning "Failure diagnostics collection failed for ${currentConfiguration}: $($_.Exception.Message)"
            }
        }

        $failedConfigurations += @{
            Configuration = $currentConfiguration
            Message = $_.Exception.Message
        }

        Write-Warning "Configuration failed: $currentConfiguration"
    }
}

if ($failedConfigurations.Count -gt 0) {
    Write-Host ''
    Write-Host 'Configuration failure summary:'
    foreach ($failedConfiguration in $failedConfigurations) {
        Write-Host "- $($failedConfiguration.Configuration): $($failedConfiguration.Message)"
    }

    throw "One or more configurations failed: $($failedConfigurations.Configuration -join ', ')"
}
