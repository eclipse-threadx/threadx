[CmdletBinding()]
param()

$ErrorActionPreference = 'Stop'

function Invoke-NativeCommand {
    param(
        [Parameter(Mandatory = $true)]
        [string]$FilePath,

        [Parameter()]
        [string[]]$Arguments = @()
    )

    & $FilePath @Arguments
    if ($LASTEXITCODE -ne 0) {
        throw "Command failed with exit code ${LASTEXITCODE}: $FilePath $($Arguments -join ' ')"
    }
}

function Get-PortSettings {
    param(
        [Parameter(Mandatory = $true)]
        [string]$SelectedArch
    )

    switch ($SelectedArch) {
        'win32' {
            return @{
                ThreadXArch = 'win32'
                ThreadXToolchain = 'vs_2019'
                VsArch = 'x86'
            }
        }
        'win64' {
            return @{
                ThreadXArch = 'win64'
                ThreadXToolchain = 'vs_2022'
                VsArch = 'amd64'
            }
        }
        default {
            throw "Unsupported architecture: $SelectedArch"
        }
    }
}

function Get-RegressionConfigurations {
    return @(
        'default_build_coverage',
        'disable_notify_callbacks_build',
        'stack_checking_build',
        'stack_checking_rand_fill_build',
        'trace_build'
    )
}

function Resolve-RegressionConfigurations {
    param(
        [Parameter(Mandatory = $false)]
        [AllowNull()]
        [AllowEmptyCollection()]
        [object]$RequestedConfigurations = 'all'
    )

    $allConfigurations = Get-RegressionConfigurations
    $resolvedConfigurations = @()

    if ($null -eq $RequestedConfigurations) {
        $resolvedConfigurations = @('all')
    }
    elseif ($RequestedConfigurations -is [System.Array]) {
        foreach ($requestedConfiguration in $RequestedConfigurations) {
            if ($null -ne $requestedConfiguration) {
                $resolvedConfigurations += [string]$requestedConfiguration
            }
        }
    }
    else {
        $resolvedConfigurations = @([string]$RequestedConfigurations)
    }

    $normalizedConfigurations = @()
    foreach ($requestedConfiguration in $resolvedConfigurations) {
        foreach ($configurationPart in ($requestedConfiguration -split ',')) {
            $trimmedConfiguration = $configurationPart.Trim()
            if ($trimmedConfiguration.Length -gt 0) {
                $normalizedConfigurations += $trimmedConfiguration
            }
        }
    }

    if (($normalizedConfigurations.Count -eq 0) -or ($normalizedConfigurations -contains 'all')) {
        return $allConfigurations
    }

    foreach ($normalizedConfiguration in $normalizedConfigurations) {
        if ($allConfigurations -notcontains $normalizedConfiguration) {
            throw "Unsupported configuration: $normalizedConfiguration"
        }
    }

    return $normalizedConfigurations
}

function Get-RegressionBuildDirectoryName {
    param(
        [Parameter(Mandatory = $true)]
        [string]$ConfigurationName
    )

    switch ($ConfigurationName) {
        'default_build_coverage' {
            return 'dbc'
        }
        'disable_notify_callbacks_build' {
            return 'dnc'
        }
        'stack_checking_build' {
            return 'sc'
        }
        'stack_checking_rand_fill_build' {
            return 'scrf'
        }
        'trace_build' {
            return 'tr'
        }
        default {
            throw "Unsupported configuration: $ConfigurationName"
        }
    }
}

function Enter-VisualStudioDevShell {
    param(
        [Parameter(Mandatory = $true)]
        [string]$VsArch
    )

    $vsWherePath = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
    if (-not (Test-Path -LiteralPath $vsWherePath)) {
        throw "Unable to locate vswhere.exe at $vsWherePath"
    }

    $installationPath = & $vsWherePath -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
    if (-not $installationPath) {
        throw 'Unable to locate a Visual Studio 2022 installation with MSVC build tools.'
    }

    $launchScript = Join-Path $installationPath 'Common7\Tools\Launch-VsDevShell.ps1'
    if (-not (Test-Path -LiteralPath $launchScript)) {
        throw "Unable to locate Launch-VsDevShell.ps1 at $launchScript"
    }

    $env:VSCMD_SKIP_SENDTELEMETRY = '1'
    & $launchScript -VsInstallationPath $installationPath -Arch $VsArch -HostArch amd64 -SkipAutomaticLocation | Out-Null

    if (-not (Get-Command cl -ErrorAction SilentlyContinue)) {
        throw 'MSVC compiler environment was not activated successfully.'
    }
}

function Remove-BuildDirectory {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Path,

        [Parameter(Mandatory = $true)]
        [string]$RepoRoot
    )

    $fullRepoRoot = [System.IO.Path]::GetFullPath($RepoRoot)
    $fullPath = [System.IO.Path]::GetFullPath($Path)

    if (-not $fullPath.StartsWith($fullRepoRoot, [System.StringComparison]::OrdinalIgnoreCase)) {
        throw "Refusing to remove a directory outside the repository: $fullPath"
    }

    if (Test-Path -LiteralPath $fullPath) {
        Remove-Item -LiteralPath $fullPath -Recurse -Force
    }
}

function Remove-NinjaLock {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Path
    )

    $ninjaLockPath = Join-Path $Path '.ninja_lock'
    if (Test-Path -LiteralPath $ninjaLockPath) {
        Remove-Item -LiteralPath $ninjaLockPath -Force
    }
}

function Invoke-ProcessWithTimeout {
    param(
        [Parameter(Mandatory = $true)]
        [string]$FilePath,

        [Parameter()]
        [string[]]$Arguments = @(),

        [Parameter()]
        [int]$TimeoutSeconds = 0
    )

    $argumentList = @()
    foreach ($argument in $Arguments) {
        if ($argument -match '\s|"') {
            $argumentList += '"' + ($argument -replace '"', '\"') + '"'
        }
        else {
            $argumentList += $argument
        }
    }

    $process = Start-Process -FilePath $FilePath -ArgumentList $argumentList -NoNewWindow -PassThru
    if ($TimeoutSeconds -le 0) {
        $process | Wait-Process
        $completed = $true
    }
    else {
        try {
            $process | Wait-Process -Timeout $TimeoutSeconds -ErrorAction Stop
            $completed = $true
        }
        catch {
            $completed = $false
        }
    }

    if (-not $completed) {
        $null = Start-Process -FilePath 'taskkill.exe' -ArgumentList @('/PID', $process.Id.ToString(), '/T', '/F') -WindowStyle Hidden -Wait -PassThru
        return @{
            Completed = $false
            ExitCode = $null
        }
    }

    $process.Refresh()
    return @{
        Completed = $true
        ExitCode = $process.ExitCode
    }
}

function Test-IsNinjaBuildDirectory {
    param(
        [Parameter(Mandatory = $true)]
        [string]$BuildDir
    )

    return (Test-Path -LiteralPath (Join-Path $BuildDir 'build.ninja'))
}

function Get-NinjaBuildStatements {
    param(
        [Parameter(Mandatory = $true)]
        [string]$BuildDir
    )

    $buildNinjaPath = Join-Path $BuildDir 'build.ninja'
    if (-not (Test-Path -LiteralPath $buildNinjaPath)) {
        throw "Unable to locate build.ninja in $BuildDir"
    }

    return Get-Content -LiteralPath $buildNinjaPath
}

function New-NinjaRspFile {
    param(
        [Parameter(Mandatory = $true)]
        [string]$BuildDir,

        [Parameter(Mandatory = $true)]
        [string]$RspRelativePath
    )

    $buildStatements = Get-NinjaBuildStatements -BuildDir $BuildDir
    $rspLine = '  RSP_FILE = ' + $RspRelativePath
    $rspIndex = -1

    for ($index = 0; $index -lt $buildStatements.Count; $index++) {
        if ($buildStatements[$index] -eq $rspLine) {
            $rspIndex = $index
            break
        }
    }

    if ($rspIndex -lt 0) {
        throw "Unable to locate RSP_FILE entry for $RspRelativePath in build.ninja."
    }

    $buildIndex = -1
    for ($index = $rspIndex; $index -ge 0; $index--) {
        if ($buildStatements[$index].StartsWith('build ')) {
            $buildIndex = $index
            break
        }
    }

    if ($buildIndex -lt 0) {
        throw "Unable to locate the build statement that owns $RspRelativePath."
    }

    $buildLine = $buildStatements[$buildIndex]
    if ($buildLine -notmatch '^build\s+\S+:\s+\S+\s+(.+)$') {
        throw "Unable to parse build statement for $RspRelativePath."
    }

    $rspContents = ($Matches[1] -split '\s+') -join [Environment]::NewLine
    $rspPath = Join-Path $BuildDir $RspRelativePath
    $rspParent = Split-Path -Parent $rspPath

    if (-not (Test-Path -LiteralPath $rspParent)) {
        New-Item -ItemType Directory -Path $rspParent | Out-Null
    }

    Set-Content -LiteralPath $rspPath -Value $rspContents
}

function Ensure-NinjaRspFiles {
    param(
        [Parameter(Mandatory = $true)]
        [string]$BuildDir,

        [Parameter(Mandatory = $true)]
        [string]$CommandLine
    )

    $rspMatches = [regex]::Matches($CommandLine, '@(?<path>[^\s"]+\.rsp)')
    foreach ($rspMatch in $rspMatches) {
        $rspRelativePath = $rspMatch.Groups['path'].Value
        $rspPath = Join-Path $BuildDir $rspRelativePath
        if (-not (Test-Path -LiteralPath $rspPath)) {
            New-NinjaRspFile -BuildDir $BuildDir -RspRelativePath $rspRelativePath
        }
    }
}

function Get-PendingNinjaCommands {
    param(
        [Parameter(Mandatory = $true)]
        [string]$BuildDir
    )

    $commandLines = & ninja -C $BuildDir -t commands
    if ($LASTEXITCODE -ne 0) {
        throw "Unable to enumerate pending Ninja commands in $BuildDir"
    }

    return $commandLines | Where-Object { -not [string]::IsNullOrWhiteSpace($_) }
}

function Invoke-NinjaFallbackBuild {
    param(
        [Parameter(Mandatory = $true)]
        [string]$BuildDir
    )

    $pendingCommands = Get-PendingNinjaCommands -BuildDir $BuildDir
    if ($pendingCommands.Count -eq 0) {
        return
    }

    Push-Location $BuildDir
    try {
        foreach ($pendingCommand in $pendingCommands) {
            Ensure-NinjaRspFiles -BuildDir $BuildDir -CommandLine $pendingCommand

            $commandToRun = $pendingCommand -replace '\s/showIncludes(?=\s|$)', ''

            if ($commandToRun -match '^[^ ]*cmd(?:\.exe)?\s+/C\s+"(?<inner>.*)"\s*$') {
                & cmd.exe /C $Matches['inner']
            }
            else {
                & cmd.exe /C $commandToRun
            }

            if ($LASTEXITCODE -ne 0) {
                throw "Fallback Ninja command failed with exit code ${LASTEXITCODE}: $pendingCommand"
            }
        }
    }
    finally {
        Pop-Location
    }
}

function Invoke-CMakeBuild {
    param(
        [Parameter(Mandatory = $true)]
        [string]$BuildDir,

        [Parameter(Mandatory = $true)]
        [int]$Parallel,

        [Parameter()]
        [int]$TimeoutSeconds = 0
    )

    Remove-NinjaLock -Path $BuildDir
    $isNinjaBuild = Test-IsNinjaBuildDirectory -BuildDir $BuildDir

    if ($TimeoutSeconds -le 0) {
        if ($isNinjaBuild) {
            Invoke-NativeCommand -FilePath 'ninja' -Arguments @(
                '-C', $BuildDir,
                '-j', $Parallel.ToString()
            )
        }
        else {
            Invoke-NativeCommand -FilePath 'cmake' -Arguments @(
                '--build', $BuildDir,
                '--parallel', $Parallel.ToString()
            )
        }
        return
    }

    if ($isNinjaBuild) {
        $buildToolName = 'Ninja'
        $buildResult = Invoke-ProcessWithTimeout -FilePath 'ninja' -Arguments @(
            '-C', $BuildDir,
            '-j', $Parallel.ToString()
        ) -TimeoutSeconds $TimeoutSeconds
    }
    else {
        $buildToolName = 'CMake'
        $buildResult = Invoke-ProcessWithTimeout -FilePath 'cmake' -Arguments @(
            '--build', $BuildDir,
            '--parallel', $Parallel.ToString()
        ) -TimeoutSeconds $TimeoutSeconds
    }

    if ($buildResult.Completed -and ($buildResult.ExitCode -eq 0)) {
        return
    }

    if (-not $isNinjaBuild) {
        if (-not $buildResult.Completed) {
            throw "$buildToolName build timed out after $TimeoutSeconds seconds in $BuildDir"
        }

        throw "$buildToolName build failed with exit code $($buildResult.ExitCode) in $BuildDir"
    }

    if ($buildResult.Completed) {
        throw "$buildToolName build failed with exit code $($buildResult.ExitCode) in $BuildDir"
    }

    Write-Warning "$buildToolName build timed out after $TimeoutSeconds seconds in $BuildDir. Replaying pending Ninja commands from PowerShell."

    Remove-NinjaLock -Path $BuildDir
    Invoke-NinjaFallbackBuild -BuildDir $BuildDir
}
