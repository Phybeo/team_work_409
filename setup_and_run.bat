@echo off
setlocal enabledelayedexpansion

REM Final robust script with enhanced diagnostics
set "MINGW_ARCHIVE=mingw64.7z"
set "MINGW_DIR=mingw64"
set "OUTPUT_EXE=PerformanceTest.exe"
set "GPP_PATH=%MINGW_DIR%\bin\g++.exe"
set "ARCHIVE_URL=https://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win64/Personal%20Builds/mingw-builds/8.1.0/threads-win32/seh/x86_64-8.1.0-release-win32-seh-rt_v6-rev0.7z/download"

REM Step 1: Verify 7-Zip installation
if exist "C:\Program Files\7-Zip\7z.exe" (
    set "SEVEN_ZIP=C:\Program Files\7-Zip\7z.exe"
) else if exist "C:\Program Files (x86)\7-Zip\7z.exe" (
    set "SEVEN_ZIP=C:\Program Files (x86)\7-Zip\7z.exe"
) else (
    echo ERROR: 7-Zip is required but not installed 
    echo 1. Download 7-Zip from https://www.7-zip.org/
    echo 2. Install using default settings
    echo 3. Run this script again
    pause
    exit /b 1
)

REM Step 2: Validate MinGW archive with detailed output
 echo [1/5] Validating MinGW archive...
echo Running: "%SEVEN_ZIP%" t %MINGW_ARCHIVE%
"%SEVEN_ZIP%" t %MINGW_ARCHIVE%
if %errorlevel% neq 0 (
    echo ERROR: MinGW archive validation failed [%errorlevel%]
    echo This indicates a corrupted or incomplete download
    echo Please perform these steps:
    echo 1. Delete the current mingw64.7z file
    echo 2. Download fresh copy from:
    echo    %ARCHIVE_URL%
    echo 3. Save it in this folder as mingw64.7z
    echo 4. Run this script again
    pause
    exit /b 1
)

REM Step 3: Extract MinGW with progress visibilit
 echo [2/5] Extracting MinGW toolchain...
if not exist "%GPP_PATH%" (
    echo Extracting: "%SEVEN_ZIP%" x %MINGW_ARCHIVE% -o%MINGW_DIR%
    "%SEVEN_ZIP%" x %MINGW_ARCHIVE% -o%MINGW_DIR%
    if %errorlevel% neq 0 (
        echo ERROR: Extraction failed [%errorlevel%]
        echo Manual extraction required:
        echo 1. Right-click mingw64.7z in File Explorer
        echo 2. Select 7-Zip ^> Extract to "mingw64\"
        echo 3. Wait for extraction to complete
        echo 4. Run this script again
        pause
        exit /b 1
    )
)

REM Step 4: Verify critical components
 echo [3/5] Verifying installation...
if not exist "%GPP_PATH%" (
    echo ERROR: Critical component missing:
    echo %GPP_PATH%
    echo This indicates a failed or incomplete extraction
    echo Please try manual extraction as instructed above
    pause
    exit /b 1
)

REM Step 5: Compile with explicit paths and error capture
 echo [4/5] Compiling application...
echo Running: "%GPP_PATH%" -std=c++11 -O2 MemoryPool.cpp PerformanceTest.cpp -o %OUTPUT_EXE% -pthread
"%GPP_PATH%" -std=c++11 -O2 MemoryPool.cpp PerformanceTest.cpp -o %OUTPUT_EXE% -pthread
if %errorlevel% neq 0 (
    echo ERROR: Compilation failed [%errorlevel%]
    echo Possible causes and solutions:
    echo 1. Missing source files - Ensure MemoryPool.cpp and PerformanceTest.cpp exist
    echo 2. Code errors - Check compiler output above for syntax issues
    echo 3. Corrupted MinGW - Delete mingw64 folder and run script again
    pause
    exit /b 1
)

REM Step 6: Execute with verification
 echo [5/5] Running performance test...
if exist "%OUTPUT_EXE%" (
    echo Executing: .\%OUTPUT_EXE%
    echo.
    .\%OUTPUT_EXE%
) else (
    echo ERROR: Executable not found after compilation
    echo This should not happen - please report this issue
    pause
    exit /b 1
)

pause
endlocal