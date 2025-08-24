@echo off
echo Building ssmtp-mailer for Windows...

REM Check if Visual Studio is available
where cl >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo Error: Visual Studio compiler (cl.exe) not found in PATH
    echo Please run this script from a Visual Studio Developer Command Prompt
    echo or run "Developer Command Prompt for VS" from Start Menu
    pause
    exit /b 1
)

REM Check if CMake is available
where cmake >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo Error: CMake not found in PATH
    echo Please install CMake and add it to your PATH
    pause
    exit /b 1
)

REM Check if OpenSSL is available
if not exist "C:\OpenSSL-Win64" (
    echo Warning: OpenSSL not found in C:\OpenSSL-Win64
    echo Please install OpenSSL for Windows or set OPENSSL_ROOT_DIR
    echo Download from: https://slproweb.com/products/Win32OpenSSL.html
)

REM Create build directory
if not exist "build" mkdir build
cd build

REM Configure with CMake
echo Configuring with CMake...
cmake .. -G "Visual Studio 17 2022" -A x64 ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DENABLE_SSL=ON ^
    -DBUILD_SHARED_LIBS=ON ^
    -DBUILD_TESTS=OFF ^
    -DBUILD_EXAMPLES=OFF

if %ERRORLEVEL% NEQ 0 (
    echo Error: CMake configuration failed
    pause
    exit /b 1
)

REM Build the project
echo Building project...
cmake --build . --config Release --parallel

if %ERRORLEVEL% NEQ 0 (
    echo Error: Build failed
    pause
    exit /b 1
)

echo.
echo Build completed successfully!
echo.
echo Executable: build\bin\Release\ssmtp-mailer.exe
echo Library: build\lib\Release\ssmtp-mailer.dll
echo.
echo To create installer package, run:
echo cmake --build . --target package
echo.

pause
