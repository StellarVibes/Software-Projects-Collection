@echo off
REM Windows 依赖安装指南
REM 此脚本提供安装 Boost_Searcher 所需依赖的详细步骤

echo ====================================
echo Boost_Searcher 依赖安装指南
echo ====================================
echo.

echo 本指南将帮助您安装以下依赖：
echo   1. CMake
echo   2. Boost 库
echo   3. JsonCpp 库
echo   4. Visual Studio (MSVC)
echo.

pause
echo.

echo ====================================
echo 第一步：安装 CMake
echo ====================================
echo.
echo 1. 访问 CMake 官网：https://cmake.org/download/
echo 2. 下载 Windows x64 Installer
echo 3. 运行安装程序，选择 "Add CMake to the system PATH"
echo 4. 完成安装后，重新打开命令提示符
echo.
echo 验证安装：运行命令 "cmake --version"
echo.
pause
echo.

echo ====================================
echo 第二步：安装 Boost 库
echo ====================================
echo.
echo 方法一：使用预编译版本（推荐）
echo   1. 访问 Boost 官网：https://www.boost.org/users/download/
echo   2. 下载 Boost 1.85.0 或更高版本的 Windows 预编译版本
echo   3. 解压到 C:\local\boost_1_85_0
echo   4. 设置环境变量 BOOST_ROOT=C:\local\boost_1_85_0
echo.
echo 方法二：从源码编译
echo   1. 下载 Boost 源码：https://www.boost.org/users/history/version_1_85_0.html
echo   2. 解压到任意目录
echo   3. 打开命令提示符，进入 Boost 目录
echo   4. 运行 bootstrap.bat
echo   5. 运行 b2 install --prefix=C:\Boost
echo   6. 设置环境变量 BOOST_ROOT=C:\Boost
echo.
pause
echo.

echo ====================================
echo 第三步：安装 JsonCpp 库
echo ====================================
echo.
echo 方法一：使用 vcpkg（推荐）
echo   1. 安装 vcpkg：
echo      git clone https://github.com/Microsoft/vcpkg.git
echo      cd vcpkg
echo      .\bootstrap-vcpkg.bat
echo   2. 安装 JsonCpp：
echo      .\vcpkg install jsoncpp:x64-windows
echo   3. 集成到 Visual Studio：
echo      .\vcpkg integrate install
echo.
echo 方法二：手动编译
echo   1. 克隆 JsonCpp 仓库：
echo      git clone https://github.com/open-source-parsers/jsoncpp.git
echo   2. 进入 JsonCpp 目录
echo   3. 创建构建目录：
echo      mkdir build && cd build
echo   4. 配置项目：
echo      cmake .. -DBUILD_SHARED_LIBS=ON -DCMAKE_INSTALL_PREFIX=C:\jsoncpp
echo   5. 编译并安装：
echo      cmake --build . --config Release
echo      cmake --install . --config Release
echo.
pause
echo.

echo ====================================
echo 第四步：安装 Visual Studio
echo ====================================
echo.
echo 1. 访问 Visual Studio 下载页面：
echo    https://visualstudio.microsoft.com/downloads/
echo 2. 下载 Visual Studio Community（免费）
echo 3. 运行安装程序
echo 4. 选择 "使用 C++ 的桌面开发" 工作负载
echo 5. 确保安装了以下组件：
echo    - MSVC v143 - VS 2022 C++ x64/x86 生成工具
echo    - Windows 10 SDK（或更高版本）
echo 6. 完成安装
echo.
pause
echo.

echo ====================================
echo 验证安装
echo ====================================
echo.
echo 请在新的命令提示符窗口中运行以下命令验证安装：
echo.
echo 1. 验证 CMake：
echo    cmake --version
echo.
echo 2. 验证 Visual Studio：
echo    cl
echo.
echo 3. 验证 Boost：
echo    dir %BOOST_ROOT%
echo.
echo 4. 验证 JsonCpp：
echo    dir C:\jsoncpp
echo.
pause
echo.

echo ====================================
echo 安装完成
echo ====================================
echo.
echo 如果所有依赖都已正确安装，您可以：
echo 1. 运行 build_windows.bat 开始编译项目
echo 2. 或手动运行 cmake -B build && cmake --build build
echo.
echo 如遇到问题，请检查：
echo - 环境变量是否正确设置
echo - 库文件路径是否正确
echo - CMakeLists.txt 中的路径配置
echo.
pause
