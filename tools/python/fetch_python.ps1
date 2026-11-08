# Fetches the pinned Python runtime for the Device Scripts compiler into
# tools\python\windows\python\ (Windows x64). The result is not committed (see
# .gitignore); MIPS.pro copies it to release\python and the release workflows
# into the deploy folder.
#
# Usage: powershell -ExecutionPolicy Bypass -File tools\python\fetch_python.ps1
$ErrorActionPreference = 'Stop'

$here = Split-Path -Parent $MyInvocation.MyCommand.Path
$v = @{}
Get-Content (Join-Path $here 'versions.txt') | ForEach-Object {
    if ($_ -match '^\s*([A-Z0-9_]+)\s*=\s*(\S+)\s*$') { $v[$Matches[1]] = $Matches[2] }
}
$dest  = Join-Path $here 'windows'
$stamp = "$($v.PY_VERSION)-embed"
$stampFile = Join-Path $dest '.version'
if ((Test-Path (Join-Path $dest 'python\python.exe')) -and (Test-Path $stampFile) -and
    ((Get-Content $stampFile -Raw).Trim() -eq $stamp)) {
    Write-Host "Python $($v.PY_VERSION) is already in $dest\python"
    exit 0
}

$name = "python-$($v.PY_VERSION)-embed-amd64.zip"
$url  = "https://www.python.org/ftp/python/$($v.PY_VERSION)/$name"
$work = Join-Path ([System.IO.Path]::GetTempPath()) ("gscpython-" + [guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory -Path $work | Out-Null
try {
    Write-Host "Downloading $name"
    $zip = Join-Path $work $name
    Invoke-WebRequest -Uri $url -OutFile $zip -UseBasicParsing

    $actual = (Get-FileHash -Algorithm SHA256 $zip).Hash.ToLower()
    if ($actual -ne $v.PY_WINDOWS_EMBED_SHA256.ToLower()) {
        throw "checksum mismatch for $name`n  expected $($v.PY_WINDOWS_EMBED_SHA256)`n  got      $actual"
    }

    $x = Join-Path $work 'python'
    Expand-Archive -Path $zip -DestinationPath $x

    # The compiler must work before it is shipped.
    & (Join-Path $x 'python.exe') -I -B -c "import argparse, difflib, json, re, struct, sys, time; assert sys.version_info >= (3, 9)"
    if ($LASTEXITCODE -ne 0) { throw "the downloaded Python does not run" }

    New-Item -ItemType Directory -Force -Path $dest | Out-Null
    $target = Join-Path $dest 'python'
    if (Test-Path $target) { Remove-Item -Recurse -Force $target }
    Move-Item $x $target
    Set-Content -Path $stampFile -Value $stamp
    Write-Host "Python $($v.PY_VERSION) is in $target"
}
finally {
    Remove-Item -Recurse -Force $work -ErrorAction SilentlyContinue
}
