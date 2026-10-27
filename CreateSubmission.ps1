[CmdletBinding()]
param(
    [ValidateSet("Debug", "Release")]
    [string]$Configuration = "Release"
)

$ErrorActionPreference = "Stop"

$projectRoot = $PSScriptRoot
$solutionPath = Join-Path $projectRoot "run.sln"
$yearRoot = Split-Path -Parent $projectRoot
$buildOutput = Join-Path $yearRoot "generated\outputs\$Configuration"
$packageRoot = Join-Path $yearRoot "generated\submission\TatariBashiri"
$runtimeDirectory = Join-Path $packageRoot "runtime"

$vswherePath = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
$vcvarsPath = $null
if (Test-Path -LiteralPath $vswherePath) {
    $visualStudioPath = & $vswherePath -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
    if ($visualStudioPath) {
        $candidate = Join-Path $visualStudioPath "VC\Auxiliary\Build\vcvars64.bat"
        if (Test-Path -LiteralPath $candidate) {
            $vcvarsPath = $candidate
        }
    }
}
if (-not $vcvarsPath) {
    throw "Visual Studio C++ build tools were not found."
}

Write-Host "[1/4] Building the game ($Configuration)..."
$buildCommand = 'call "{0}" && msbuild "{1}" /m /p:Configuration={2} /p:Platform=x64' -f $vcvarsPath, $solutionPath, $Configuration
& $env:ComSpec /d /c $buildCommand
if ($LASTEXITCODE -ne 0) {
    throw "The game build failed."
}

$gameExe = Join-Path $buildOutput "run.exe"
$resourceSource = Join-Path $projectRoot "resources"
if (-not (Test-Path -LiteralPath $gameExe)) {
    throw "The game executable was not found: $gameExe"
}
if (-not (Test-Path -LiteralPath $resourceSource)) {
    throw "The resources folder was not found: $resourceSource"
}

Write-Host "[2/4] Creating the submission package..."
if (Test-Path -LiteralPath $packageRoot) {
    Remove-Item -LiteralPath $packageRoot -Recurse -Force
}
New-Item -ItemType Directory -Path $runtimeDirectory -Force | Out-Null

Copy-Item -LiteralPath $gameExe -Destination (Join-Path $runtimeDirectory "TatariBashiriGame.exe")

$runtimeDlls = @(
    "assimp-vc145-mt.dll",
    "dxcompiler.dll",
    "dxil.dll",
    "kubazip.dll",
    "minizip.dll",
    "poly2tri.dll",
    "pugixml.dll",
    "zlib1.dll"
)

foreach ($dllName in $runtimeDlls) {
    $dllPath = Join-Path $buildOutput $dllName
    if (-not (Test-Path -LiteralPath $dllPath)) {
        throw "A required DLL was not found: $dllPath"
    }
    Copy-Item -LiteralPath $dllPath -Destination $runtimeDirectory
}

Copy-Item -LiteralPath $resourceSource -Destination (Join-Path $packageRoot "resources") -Recurse

Write-Host "[3/4] Building the launcher..."
$launcherSource = Join-Path $projectRoot "tools\SubmissionLauncher.cpp"
$launcherOutput = Join-Path $packageRoot "[PLAY] TatariBashiri.exe"
$launcherObject = Join-Path $env:TEMP "TatariBashiriSubmissionLauncher.obj"
$launcherCommand = 'call "{0}" && cl /nologo /std:c++20 /O2 /EHsc /utf-8 /DUNICODE /D_UNICODE /Fo:"{1}" "{2}" shell32.lib user32.lib /link /SUBSYSTEM:WINDOWS /OUT:"{3}"' -f $vcvarsPath, $launcherObject, $launcherSource, $launcherOutput
& $env:ComSpec /d /c $launcherCommand
if ($LASTEXITCODE -ne 0) {
    throw "The launcher build failed."
}
Remove-Item -LiteralPath $launcherObject -Force -ErrorAction SilentlyContinue

$readme = @"
Double-click [PLAY] TatariBashiri.exe to start the game.

The runtime and resources folders are required.
Submit the entire TatariBashiri folder without moving or deleting its contents.
"@
Set-Content -LiteralPath (Join-Path $packageRoot "README.txt") -Value $readme -Encoding ASCII

Write-Host "[4/4] Done."
Write-Host "Submission folder: $packageRoot" -ForegroundColor Green
