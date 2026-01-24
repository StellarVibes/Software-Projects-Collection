#!/bin/bash
# Boost_Searcher Linux 构建脚本
# 用于在 Linux 平台上编译项目

echo "===================================="
echo "Boost_Searcher Linux 构建脚本"
echo "===================================="
echo

# 检查 CMake 是否安装
if ! command -v cmake &> /dev/null; then
    echo "[错误] 未找到 CMake，请先安装 CMake"
    echo "Ubuntu/Debian: sudo apt-get install cmake"
    echo "CentOS/RHEL: sudo yum install cmake"
    exit 1
fi

echo "[信息] CMake 已安装"

# 检查 g++ 是否安装
if ! command -v g++ &> /dev/null; then
    echo "[错误] 未找到 g++，请先安装 g++"
    echo "Ubuntu/Debian: sudo apt-get install g++"
    echo "CentOS/RHEL: sudo yum install gcc-c++"
    exit 1
fi

echo "[信息] g++ 已安装"

# 检查 Boost 是否安装
if ! pkg-config --exists libboost_system 2>/dev/null; then
    echo "[警告] 未找到 Boost 库，请确保 Boost 已正确安装"
    echo "Ubuntu/Debian: sudo apt-get install libboost-all-dev"
    echo "CentOS/RHEL: sudo yum install boost-devel"
    echo "或从源码编译 Boost: https://www.boost.org/doc/libs/release/"
fi

# 检查 JsonCpp 是否安装
if ! pkg-config --exists jsoncpp 2>/dev/null; then
    echo "[警告] 未找到 JsonCpp 库，请确保 JsonCpp 已正确安装"
    echo "Ubuntu/Debian: sudo apt-get install libjsoncpp-dev"
    echo "CentOS/RHEL: sudo yum install jsoncpp-devel"
    echo "或从源码编译 JsonCpp: https://github.com/open-source-parsers/jsoncpp"
fi

echo
echo "[信息] 开始配置项目..."
cmake -B build
if [ $? -ne 0 ]; then
    echo "[错误] CMake 配置失败"
    exit 1
fi

echo "[成功] CMake 配置成功"
echo
echo "[信息] 开始编译项目..."
cmake --build build
if [ $? -ne 0 ]; then
    echo "[错误] 编译失败"
    exit 1
fi

echo
echo "===================================="
echo "[成功] 编译完成！"
echo "===================================="
echo
echo "可执行文件位置:"
echo "  - parser:       解析 HTML 文件"
echo "  - debug:        调试搜索功能"
echo "  - http_server:  HTTP 搜索服务器"
echo "  - test_jieba:   测试分词功能"
echo
echo "使用说明:"
echo "  1. 运行 test_jieba 测试分词功能"
echo "  2. 运行 parser 解析数据"
echo "  3. 运行 http_server 启动服务器"
echo "  4. 在浏览器访问 http://localhost:8080"
echo
