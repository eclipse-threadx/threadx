[CmdletBinding()]
param(
    [AllowNull()]
    [object]$Configuration = 'all',

    [int]$Parallel = [Math]::Max(1, [Environment]::ProcessorCount),

    [int]$BuildTimeoutSeconds = 180,

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

foreach ($currentConfiguration in $selectedConfigurations) {
    $currentBuildDirName = Get-RegressionBuildDirectoryName -ConfigurationName $currentConfiguration
    $currentBuildDir = Join-Path $BuildDir $currentBuildDirName

    if ($Clean) {
        Remove-BuildDirectory -Path $currentBuildDir -RepoRoot $repoRoot
    }

    Remove-NinjaLock -Path $currentBuildDir

    Write-Host "Configuring win64_smp / $currentConfiguration"
    Invoke-NativeCommand -FilePath 'cmake' -Arguments @(
        '-S', (Join-Path $repoRoot 'test\smp\cmake'),
        '-B', $currentBuildDir,
        '-G', 'Ninja',
        '-DCMAKE_C_COMPILER_FORCED=TRUE',
        '-DCMAKE_C_COMPILER_WORKS=TRUE',
        '-DCMAKE_C_ABI_COMPILED=TRUE',
        "-DCMAKE_BUILD_TYPE=$currentConfiguration",
        '-DTHREADX_ARCH=win64',
        '-DTHREADX_TOOLCHAIN=vs_2022'
    )

    Write-Host "Building win64_smp / $currentConfiguration"
    Invoke-CMakeBuild -BuildDir $currentBuildDir -Parallel $Parallel -TimeoutSeconds $BuildTimeoutSeconds
}
