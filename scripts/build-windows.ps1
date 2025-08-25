# PowerShell script for building ssmtp-mailer on Windows
# Run with: powershell -ExecutionPolicy Bypass -File build-windows.ps1

Write-Host "Building ssmtp-mailer for Windows..." -ForegroundColor Green

# Check if Visual Studio is available
try {
    $null = Get-Command cl -ErrorAction Stop
    Write-Host "✓ Visual Studio compiler found" -ForegroundColor Green
} catch {
    Write-Host "✗ Error: Visual Studio compiler (cl.exe) not found in PATH" -ForegroundColor Red
    Write-Host "Please run this script from a Visual Studio Developer Command Prompt" -ForegroundColor Yellow
    Write-Host "or run 'Developer Command Prompt for VS' from Start Menu" -ForegroundColor Yellow
    Read-Host "Press Enter to exit"
    exit 1
}

# Check if CMake is available
try {
    $null = Get-Command cmake -ErrorAction Stop
    Write-Host "✓ CMake found" -ForegroundColor Green
} catch {
    Write-Host "✗ Error: CMake not found in PATH" -ForegroundColor Red
    Write-Host "Please install CMake and add it to your PATH" -ForegroundColor Yellow
    Read-Host "Press Enter to exit"
    exit 1
}

# Check if OpenSSL is available
if (Test-Path "C:\OpenSSL-Win64") {
    Write-Host "✓ OpenSSL found in C:\OpenSSL-Win64" -ForegroundColor Green
} else {
    Write-Host "⚠ Warning: OpenSSL not found in C:\OpenSSL-Win64" -ForegroundColor Yellow
    Write-Host "Please install OpenSSL for Windows or set OPENSSL_ROOT_DIR" -ForegroundColor Yellow
    Write-Host "Download from: https://slproweb.com/products/Win32OpenSSL.html" -ForegroundColor Yellow
}

# Create build directory
if (-not (Test-Path "build")) {
    New-Item -ItemType Directory -Path "build" | Out-Null
}
Set-Location build

# Configure with CMake
Write-Host "Configuring with CMake..." -ForegroundColor Cyan
$cmakeArgs = @(
    "..",
    "-G", "Visual Studio 17 2022",
    "-A", "x64",
    "-DCMAKE_BUILD_TYPE=Release",
    "-DENABLE_SSL=ON",
    "-DBUILD_SHARED_LIBS=ON",
    "-DBUILD_TESTS=OFF",
    "-DBUILD_EXAMPLES=OFF"
)

$cmakeResult = & cmake @cmakeArgs
if ($LASTEXITCODE -ne 0) {
    Write-Host "✗ Error: CMake configuration failed" -ForegroundColor Red
    Read-Host "Press Enter to exit"
    exit 1
}

# Build the project
Write-Host "Building project..." -ForegroundColor Cyan
$buildResult = & cmake --build . --config Release --parallel
if ($LASTEXITCODE -ne 0) {
    Write-Host "✗ Error: Build failed" -ForegroundColor Red
    Read-Host "Press Enter to exit"
    exit 1
}

Write-Host ""
Write-Host "✓ Build completed successfully!" -ForegroundColor Green
Write-Host ""
Write-Host "Executable: build\bin\Release\ssmtp-mailer.exe" -ForegroundColor White
Write-Host "Library: build\lib\Release\ssmtp-mailer.dll" -ForegroundColor White
Write-Host ""
Write-Host "To create installer package, run:" -ForegroundColor Cyan
Write-Host "cmake --build . --target package" -ForegroundColor White
Write-Host ""

Read-Host "Press Enter to exit"
