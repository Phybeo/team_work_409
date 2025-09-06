@echo off
setlocal enabledelayedexpansion

REM 尝试使用不同的C++编译器来编译性能测试程序
set COMPILE_SUCCESS=0

REM 检查源文件是否存在
echo 检查必要的源文件...
if not exist "MemoryPool.cpp" (
    echo 错误: 找不到源文件 MemoryPool.cpp
    echo 当前目录: %cd%
    echo 请确保在项目目录中运行此脚本
    goto end
)
if not exist "PerformanceTest.cpp" (
    echo 错误: 找不到源文件 PerformanceTest.cpp
    echo 当前目录: %cd%
    echo 请确保在项目目录中运行此脚本
    goto end
)

REM 显示调试信息
echo 编译脚本开始执行...
echo 当前目录: %cd%

echo 检查是否有可用的编译器...

REM 检查GCC编译器
where g++ >nul 2>nul
if %errorlevel% equ 0 (
    echo 找到GCC编译器
    echo 使用GCC编译器编译...
    g++ -std=c++11 -O2 MemoryPool.cpp PerformanceTest.cpp -o PerformanceTest.exe -pthread
    if %errorlevel% equ 0 (
        echo GCC编译成功！生成了PerformanceTest.exe
        echo 运行性能测试请执行: .\PerformanceTest.exe
        set COMPILE_SUCCESS=1
        goto end
    ) else (
        echo GCC编译失败，错误代码: %errorlevel%
        echo 尝试其他编译器...
    )
)

REM 检查MSVC编译器
where cl >nul 2>nul
if %errorlevel% equ 0 (
    echo 找到MSVC编译器
    echo 使用MSVC编译器编译...
    cl /EHsc /O2 MemoryPool.cpp PerformanceTest.cpp /FePerformanceTest.exe > compile_msvc.log 2>&1
    if %errorlevel% equ 0 (
        echo MSVC编译成功！生成了PerformanceTest.exe
        echo 运行性能测试请执行: .\PerformanceTest.exe
        set COMPILE_SUCCESS=1
        goto end
    ) else (
        echo MSVC编译失败，错误代码: %errorlevel%
        echo 详细错误信息已保存到compile_msvc.log
        echo 尝试其他方法...
    )
)

REM 检查Visual Studio环境变量 - 支持多个版本
set VS_VERSIONS=17 16 15 14
set VCVARS_FOUND=0

echo 检查Visual Studio环境...
for %%v in (%VS_VERSIONS%) do (
    if defined VS%%v%%COMNTOOLS (
        set "VCVARS_PATH=!VS%%v%%COMNTOOLS!..\..\VC\Auxiliary\Build\vcvars64.bat"
        if exist "!VCVARS_PATH!" (
            echo 找到Visual Studio %%v环境: !VS%%v%%COMNTOOLS!
            call "!VCVARS_PATH!" >nul 2>&1
            if !errorlevel! equ 0 (
                echo 成功设置Visual Studio %%v环境
                echo 验证编译器是否可用...
                where cl >nul 2>nul
                if !errorlevel! equ 0 (
                    echo 使用MSVC编译器编译...
                    cl /EHsc /O2 MemoryPool.cpp PerformanceTest.cpp /FePerformanceTest.exe > compile_vs%%v.log 2>&1
                    if !errorlevel! equ 0 (
                        echo MSVC编译成功！生成了PerformanceTest.exe
                        echo 运行性能测试请执行: .\PerformanceTest.exe
                        set COMPILE_SUCCESS=1
                        set VCVARS_FOUND=1
                        goto end
                    ) else (
                        echo MSVC编译失败，错误代码: !errorlevel!
                        echo 详细错误信息已保存到compile_vs%%v.log
                    )
                ) else (
                    echo 设置环境后未找到编译器cl.exe
                )
            ) else (
                echo 设置Visual Studio %%v环境失败，错误代码: !errorlevel!
            )
        ) else (
            echo 找不到vcvars64.bat: !VCVARS_PATH!
        )
    )
)

REM 如果没有找到编译器，提示用户
if %COMPILE_SUCCESS% equ 0 (
    echo 编译失败！未找到可用的C++编译器或编译过程出错。
    echo 请尝试以下解决方法:
    echo 1. 安装MinGW-w64 (GCC)并确保已添加到环境变量
    echo    下载地址: https://sourceforge.net/projects/mingw-w64/
    echo 2. 安装Visual Studio 2019或更高版本，并勾选"C++桌面开发"工作负载
    echo    下载地址: https://visualstudio.microsoft.com/
    echo 3. 使用Visual Studio Developer Command Prompt运行此脚本
)

:end
if %COMPILE_SUCCESS% equ 1 (
    echo 编译脚本执行成功
) else (
    echo 编译脚本执行失败
)
endlocal
pause