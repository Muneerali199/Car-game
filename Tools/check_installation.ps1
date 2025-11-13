# Installation Verification Script
# Run this after installing VS2022 and UE5

Write-Host "`n=== Checking Installation Status ===" -ForegroundColor Cyan

# Check Visual Studio C++ Tools
Write-Host "`n1. Checking Visual Studio C++ Tools..." -ForegroundColor Yellow
$vsPaths = @(
    "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC",
    "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Tools\MSVC",
    "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Tools\MSVC"
)

$vsFound = $false
foreach ($path in $vsPaths) {
    if (Test-Path $path) {
        Write-Host "   ✅ Visual Studio C++ Tools found at: $path" -ForegroundColor Green
        $vsFound = $true
        break
    }
}

if (-not $vsFound) {
    Write-Host "   ❌ Visual Studio C++ Tools NOT found" -ForegroundColor Red
    Write-Host "   → Install 'Game development with C++' workload" -ForegroundColor Yellow
}

# Check Unreal Engine
Write-Host "`n2. Checking Unreal Engine 5..." -ForegroundColor Yellow
$uePaths = Get-ChildItem "C:\Program Files\Epic Games" -Directory -ErrorAction SilentlyContinue | Where-Object { $_.Name -match "UE_5" }

if ($uePaths) {
    foreach ($uePath in $uePaths) {
        Write-Host "   ✅ Unreal Engine found: $($uePath.Name)" -ForegroundColor Green
        Write-Host "      Location: $($uePath.FullName)" -ForegroundColor Gray
    }
} else {
    Write-Host "   ❌ Unreal Engine 5 NOT found" -ForegroundColor Red
    Write-Host "   → Install via Epic Games Launcher" -ForegroundColor Yellow
}

# Check Git
Write-Host "`n3. Checking Git..." -ForegroundColor Yellow
try {
    $gitVersion = git --version
    Write-Host "   ✅ Git installed: $gitVersion" -ForegroundColor Green
} catch {
    Write-Host "   ❌ Git NOT found" -ForegroundColor Red
}

# Check Git LFS
Write-Host "`n4. Checking Git LFS..." -ForegroundColor Yellow
try {
    $lfsVersion = git lfs version
    Write-Host "   ✅ Git LFS installed: $lfsVersion" -ForegroundColor Green
} catch {
    Write-Host "   ❌ Git LFS NOT found" -ForegroundColor Red
}

Write-Host "`n=== Installation Check Complete ===" -ForegroundColor Cyan
Write-Host "`nIf all items show ✅, you're ready to build the project!" -ForegroundColor Green
Write-Host "If any show ❌, please install the missing component.`n" -ForegroundColor Yellow
