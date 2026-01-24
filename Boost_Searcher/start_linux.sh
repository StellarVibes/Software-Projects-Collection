#!/bin/bash
# Boost_Searcher Linux 快速启动脚本
# 用于快速启动项目的所有服务

echo "===================================="
echo "Boost_Searcher 快速启动脚本"
echo "===================================="
echo

# 检查可执行文件是否存在
if [ ! -f "./build/parser" ]; then
    echo "[错误] 未找到 parser 可执行文件"
    echo "请先运行 ./build_linux.sh 编译项目"
    exit 1
fi

if [ ! -f "./build/http_server" ]; then
    echo "[错误] 未找到 http_server 可执行文件"
    echo "请先运行 ./build_linux.sh 编译项目"
    exit 1
fi

# 检查数据文件是否存在
if [ ! -f "./data/raw_html/raw.txt" ]; then
    echo "[警告] 未找到数据文件 data/raw_html/raw.txt"
    echo "是否需要先运行 parser 解析数据？(y/n)"
    read -r response
    if [[ "$response" =~ ^[Yy]$ ]]; then
        echo "[信息] 正在运行 parser 解析数据..."
        ./build/parser
        if [ $? -ne 0 ]; then
            echo "[错误] parser 运行失败"
            exit 1
        fi
        echo "[成功] 数据解析完成"
    fi
fi

# 检查端口是否被占用
if netstat -tuln 2>/dev/null | grep -q ":8080 "; then
    echo "[警告] 端口 8080 已被占用"
    echo "是否继续启动？(y/n)"
    read -r response
    if [[ ! "$response" =~ ^[Yy]$ ]]; then
        echo "[信息] 取消启动"
        exit 0
    fi
fi

# 启动 HTTP 服务器
echo "[信息] 正在启动 HTTP 服务器..."
echo
echo "===================================="
echo "服务信息"
echo "===================================="
echo "服务名称：Boost 搜索引擎"
echo "监听端口：8080"
echo "监听地址：0.0.0.0"
echo "访问地址：http://localhost:8080"
echo "===================================="
echo
echo "[信息] 按 Ctrl+C 停止服务"
echo

# 启动服务器
./build/http_server

# 捕获 Ctrl+C 信号
trap 'echo; echo "[信息] 服务已停止"; exit 0' INT
