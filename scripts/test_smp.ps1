[CmdletBinding()]
param(
    [AllowNull()]
    [object]$Configuration = 'all',

    [int]$Parallel = 1,

    [int]$RepeatFailCount = 2,

    [int]$TestTimeoutSeconds = 45,

    [switch]$CollectFailureDiagnostics = $true,

    [string]$TestRegex,

    [switch]$RerunFailedOnly,

    [string]$BuildDir,

    [switch]$Clean
)

$ErrorActionPreference = 'Stop'
. (Join-Path $PSScriptRoot 'tx_windows_common.ps1')

$repoRoot = Split-Path -Parent $PSScriptRoot

if (-not $BuildDir) {
    $BuildDir = Join-Path $repoRoot 'build\tests\win64_smp'
}

$selectedConfigurations = Resolve-RegressionConfigurations -RequestedConfigurations $Configuration
Write-Host "Selected configurations: $($selectedConfigurations -join ', ')"

Enter-VisualStudioDevShell -VsArch 'amd64'

if ($Parallel -ne 1) {
    Write-Warning 'Windows SMP simulator regression tests are timing-sensitive. Forcing -Parallel 1.'
    $Parallel = 1
}

if ($TestRegex -and -not $PSBoundParameters.ContainsKey('TestTimeoutSeconds')) {
    $TestTimeoutSeconds = 60
    Write-Host "Targeted run detected; using per-test timeout of $TestTimeoutSeconds seconds."
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
            throw "Build directory does not exist for win64_smp / ${currentConfiguration}: $currentBuildDir"
        }

        Remove-NinjaLock -Path $currentBuildDir
        if (Test-Path -LiteralPath $currentTestingTemporaryDir) {
            Remove-Item -LiteralPath (Join-Path $currentTestingTemporaryDir 'LastTest.log') -Force -ErrorAction SilentlyContinue
            Remove-Item -LiteralPath (Join-Path $currentTestingTemporaryDir 'LastTestsFailed.log') -Force -ErrorAction SilentlyContinue
        }

        Write-Host "Testing win64_smp / $currentConfiguration"
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
