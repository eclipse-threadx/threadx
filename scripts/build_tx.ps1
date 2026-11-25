[CmdletBinding()]
param(
    [ValidateSet('win64', 'win32')]
    [string]$Arch = 'win64',

    [AllowNull()]
    [object]$Configuration = 'all',

    [int]$Parallel = [Math]::Max(1, [Environment]::ProcessorCount),

    [int]$BuildTimeoutSeconds = 60,

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

foreach ($currentConfiguration in $selectedConfigurations) {
    $currentBuildDirName = Get-RegressionBuildDirectoryName -ConfigurationName $currentConfiguration
    $currentBuildDir = Join-Path $BuildDir $currentBuildDirName

    if ($Clean) {
        Remove-BuildDirectory -Path $currentBuildDir -RepoRoot $repoRoot
    }

    Remove-NinjaLock -Path $currentBuildDir

    Write-Host "Configuring $Arch / $currentConfiguration"
    Invoke-NativeCommand -FilePath 'cmake' -Arguments @(
        '-S', (Join-Path $repoRoot 'test\tx\cmake'),
        '-B', $currentBuildDir,
        '-G', 'Ninja',
        '-DCMAKE_C_COMPILER_FORCED=TRUE',
        '-DCMAKE_C_COMPILER_WORKS=TRUE',
        '-DCMAKE_C_ABI_COMPILED=TRUE',
        "-DCMAKE_BUILD_TYPE=$currentConfiguration",
        "-DTHREADX_ARCH=$($settings.ThreadXArch)",
        "-DTHREADX_TOOLCHAIN=$($settings.ThreadXToolchain)"
    )

    Write-Host "Building $Arch / $currentConfiguration"
    Invoke-CMakeBuild -BuildDir $currentBuildDir -Parallel $Parallel -TimeoutSeconds $BuildTimeoutSeconds
}
