@echo off
REM Windows 编译脚本
REM 需要安装以下依赖：
REM 1. Visual Studio (MSVC)
REM 2. Boost 库
REM 3. JsonCpp 库
REM 4. CMake

echo ====================================
echo Boost_Searcher Windows 编译脚本
echo ====================================
echo.

REM 检查 CMake 是否安装
where cmake >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo [错误] 未找到 CMake，请先安装 CMake
    echo 下载地址: https://cmake.org/download/
    pause
    exit /b 1
)

echo [信息] CMake 已安装

REM 检查 Boost 是否安装
if not exist "C:\local\boost_1_85_0" (
    if not exist "C:\Program Files\boost" (
        echo [警告] 未找到 Boost 库，请确保 Boost 已正确安装
        echo 下载地址: https://www.boost.org/users/download/
        echo.
        echo 如果 Boost 安装在其他位置，请修改 CMakeLists.txt 中的 Boost 路径
    )
)

REM 检查 JsonCpp 是否安装
if not exist "C:\Program Files\jsoncpp" (
    echo [警告] 未找到 JsonCpp 库，请确保 JsonCpp 已正确安装
    echo 下载地址: https://github.com/open-source-parsers/jsoncpp
    echo.
    echo 如果 JsonCpp 安装在其他位置，请修改 CMakeLists.txt 中的 JsonCpp 路径
)

echo.
echo [信息] 开始配置项目...
cmake -B build
if %ERRORLEVEL% NEQ 0 (
    echo [错误] CMake 配置失败
    pause
    exit /b 1
)

echo [信息] CMake 配置成功
echo.
echo [信息] 开始编译项目...
cmake --build build --config Release
if %ERRORLEVEL% NEQ 0 (
    echo [错误] 编译失败
    pause
    exit /b 1
)

echo.
echo ====================================
echo [成功] 编译完成！
echo ====================================
echo.
echo 可执行文件位置:
echo   - parser.exe:       解析 HTML 文件
echo   - debug.exe:        调试搜索功能
echo   - http_server.exe:  HTTP 搜索服务器
echo   - test_jieba.exe:   测试分词功能
echo.
echo 使用说明:
echo   1. 运行 test_jieba.exe 测试分词功能
echo   2. 运行 parser.exe 解析数据
echo   3. 运行 http_server.exe 启动服务器
echo   4. 在浏览器访问 http://localhost:8080
echo.
pause
