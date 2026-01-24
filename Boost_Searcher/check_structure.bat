@echo off
REM 快速测试脚本 - 验证项目结构和依赖

echo ====================================
echo Boost_Searcher 项目结构检查
echo ====================================
echo.

echo [1/5] 检查源代码文件...
if exist "parser.cpp" (
    echo [OK] parser.cpp 存在
) else (
    echo [错误] parser.cpp 不存在
    goto :error
)

if exist "http_server.cpp" (
    echo [OK] http_server.cpp 存在
) else (
    echo [错误] http_server.cpp 不存在
    goto :error
)

if exist "debug.cpp" (
    echo [OK] debug.cpp 存在
) else (
    echo [错误] debug.cpp 不存在
    goto :error
)

if exist "test_jieba.cpp" (
    echo [OK] test_jieba.cpp 存在
) else (
    echo [错误] test_jieba.cpp 不存在
    goto :error
)

if exist "index.hpp" (
    echo [OK] index.hpp 存在
) else (
    echo [错误] index.hpp 不存在
    goto :error
)

if exist "searcher.hpp" (
    echo [OK] searcher.hpp 存在
) else (
    echo [错误] searcher.hpp 不存在
    goto :error
)

if exist "util.hpp" (
    echo [OK] util.hpp 存在
) else (
    echo [错误] util.hpp 不存在
    goto :error
)

if exist "log.hpp" (
    echo [OK] log.hpp 存在
) else (
    echo [错误] log.hpp 不存在
    goto :error
)

echo.
echo [2/5] 检查 cppjieba 分词库...
if exist "cppjieba\Jieba.hpp" (
    echo [OK] cppjieba/Jieba.hpp 存在
) else (
    echo [错误] cppjieba/Jieba.hpp 不存在
    goto :error
)

if exist "cppjieba\dict\jieba.dict.utf8" (
    echo [OK] cppjieba/dict/jieba.dict.utf8 存在
) else (
    echo [错误] cppjieba/dict/jieba.dict.utf8 不存在
    goto :error
)

if exist "cppjieba\dict\hmm_model.utf8" (
    echo [OK] cppjieba/dict/hmm_model.utf8 存在
) else (
    echo [错误] cppjieba/dict/hmm_model.utf8 不存在
    goto :error
)

if exist "cppjieba\dict\user.dict.utf8" (
    echo [OK] cppjieba/dict/user.dict.utf8 存在
) else (
    echo [错误] cppjieba/dict/user.dict.utf8 不存在
    goto :error
)

if exist "cppjieba\dict\idf.utf8" (
    echo [OK] cppjieba/dict/idf.utf8 存在
) else (
    echo [错误] cppjieba/dict/idf.utf8 不存在
    goto :error
)

if exist "cppjieba\dict\stop_words.utf8" (
    echo [OK] cppjieba/dict/stop_words.utf8 存在
) else (
    echo [错误] cppjieba/dict/stop_words.utf8 不存在
    goto :error
)

echo.
echo [3/5] 检查 cpp-httplib 库...
if exist "cpp-httplib\httplib.h" (
    echo [OK] cpp-httplib/httplib.h 存在
) else (
    echo [错误] cpp-httplib/httplib.h 不存在
    goto :error
)

echo.
echo [4/5] 检查数据目录...
if exist "data\input" (
    echo [OK] data/input 目录存在
) else (
    echo [警告] data/input 目录不存在（可能没有输入数据）
)

if exist "data\raw_html" (
    echo [OK] data/raw_html 目录存在
) else (
    echo [警告] data/raw_html 目录不存在（运行 parser.exe 后会自动创建）
)

echo.
echo [5/5] 检查 Web 资源...
if exist "wwwroot\index.html" (
    echo [OK] wwwroot/index.html 存在
) else (
    echo [警告] wwwroot/index.html 不存在
)

echo.
echo ====================================
echo [成功] 项目结构检查完成！
echo ====================================
echo.
echo 下一步：
echo 1. 运行 install_dependencies.bat 安装依赖
echo 2. 运行 build_windows.bat 编译项目
echo 3. 运行 test_jieba.exe 测试分词功能
echo 4. 运行 parser.exe 解析数据
echo 5. 运行 http_server.exe 启动服务器
echo.
pause
exit /b 0

:error
echo.
echo ====================================
echo [错误] 项目结构检查失败！
echo ====================================
echo.
echo 请确保所有必需的文件都存在。
pause
exit /b 1
