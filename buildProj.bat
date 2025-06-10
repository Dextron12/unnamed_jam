@echo off
setlocal

REM Check if argument is provided
if "%~1"=="" (
    echo Usage: BuildProj.bat ^<command^>
    echo Commands:
    echo   configure - generate build files
    echo   build     - build the project
    echo   buildrun  - build and run application
    echo   clean     - clean build directory
    echo   run       - run the executable
    exit /b 1
)

set CMD=%1
set BUILD_DIR=build
set EXE=%BUILD_DIR%\MyProjectExe.exe

if /I "%CMD%"=="configure" (
    echo Configuring project...
    echo %VCPKG_ROOT%
    cmake -B %BUILD_DIR% -S . -DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows
    if errorlevel 1 (
        echo Configuration failed!
        exit /b 1
    )
    goto end
)

if /I "%CMD%"=="build" (
    echo Building project...
    cmake --build %BUILD_DIR%
    if errorlevel 1 (
        echo Build failed!
        exit /b 1
    )
    goto end
)

if /I "%CMD%"=="clean" (
    echo Cleaning build directory...
    if exist %BUILD_DIR% (
        rmdir /s /q %BUILD_DIR%
        if errorlevel 1 (
            echo Failed to clean build directory.
            exit /b 1
        )
        echo Build directory cleaned.
    ) else (
        echo Build directory does not exist.
    )
    goto end
)

if /I "%CMD%"=="run" (
    if exist "%EXE%" (
        echo Running executable...
        "%EXE%"
    ) else (
        echo Executable not found. Build the project first.
        exit /b 1
    )
    goto end
)

if /I "%CMD%"=="buildrun" (
    echo "Building project"
    cmake --build build --config Release
    if errorlevel 1 (
        echo "Build failed. Review errors."
        exit /b 1
    )
    if exist "%EXE%" (
        echo "Running executable..."
        "%EXE%"
    )
    goto end
)

echo Unknown command: %CMD%
exit /b 1

:end
endlocal

