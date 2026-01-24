#!/bin/bash
# Boost_Searcher Linux 依赖安装指南
# 此脚本提供安装 Boost_Searcher 所需依赖的详细步骤

echo "===================================="
echo "Boost_Searcher 依赖安装指南"
echo "===================================="
echo

echo "本指南将帮助您安装以下依赖："
echo "  1. CMake"
echo "  2. g++ (GCC)"
echo "  3. Boost 库"
echo "  4. JsonCpp 库"
echo

# 检测 Linux 发行版
if [ -f /etc/os-release ]; then
    . /etc/os-release
    OS=$ID
    VERSION=$VERSION_ID
else
    OS=$(uname -s)
    VERSION=$(uname -r)
fi

echo "检测到的操作系统: $OS $VERSION"
echo

# 安装 CMake
echo "===================================="
echo "第一步：安装 CMake"
echo "===================================="
echo

if command -v cmake &> /dev/null; then
    echo "[信息] CMake 已安装"
    cmake --version
else
    echo "[信息] 正在安装 CMake..."
    case $OS in
        ubuntu|debian)
            sudo apt-get update
            sudo apt-get install -y cmake
            ;;
        centos|rhel|fedora)
            sudo yum install -y cmake
            ;;
        arch)
            sudo pacman -S cmake
            ;;
        *)
            echo "[错误] 不支持的操作系统: $OS"
            echo "请手动安装 CMake: https://cmake.org/download/"
            exit 1
            ;;
    esac
    
    if [ $? -eq 0 ]; then
        echo "[成功] CMake 安装成功"
        cmake --version
    else
        echo "[错误] CMake 安装失败"
        exit 1
    fi
fi

echo

# 安装 g++
echo "===================================="
echo "第二步：安装 g++ (GCC)"
echo "===================================="
echo

if command -v g++ &> /dev/null; then
    echo "[信息] g++ 已安装"
    g++ --version | head -1
else
    echo "[信息] 正在安装 g++..."
    case $OS in
        ubuntu|debian)
            sudo apt-get install -y build-essential g++
            ;;
        centos|rhel|fedora)
            sudo yum groupinstall -y "Development Tools"
            sudo yum install -y gcc-c++
            ;;
        arch)
            sudo pacman -S base-devel gcc
            ;;
        *)
            echo "[错误] 不支持的操作系统: $OS"
            echo "请手动安装 g++"
            exit 1
            ;;
    esac
    
    if [ $? -eq 0 ]; then
        echo "[成功] g++ 安装成功"
        g++ --version | head -1
    else
        echo "[错误] g++ 安装失败"
        exit 1
    fi
fi

echo

# 安装 Boost 库
echo "===================================="
echo "第三步：安装 Boost 库"
echo "===================================="
echo

if pkg-config --exists libboost_system 2>/dev/null; then
    echo "[信息] Boost 库已安装"
    pkg-config --modversion libboost_system
else
    echo "[信息] 正在安装 Boost 库..."
    case $OS in
        ubuntu|debian)
            sudo apt-get install -y libboost-all-dev
            ;;
        centos|rhel|fedora)
            sudo yum install -y boost-devel
            ;;
        arch)
            sudo pacman -S boost
            ;;
        *)
            echo "[错误] 不支持的操作系统: $OS"
            echo "请手动安装 Boost 库"
            exit 1
            ;;
    esac
    
    if [ $? -eq 0 ]; then
        echo "[成功] Boost 库安装成功"
        pkg-config --modversion libboost_system
    else
        echo "[错误] Boost 库安装失败"
        exit 1
    fi
fi

echo

# 安装 JsonCpp 库
echo "===================================="
echo "第四步：安装 JsonCpp 库"
echo "===================================="
echo

if pkg-config --exists jsoncpp 2>/dev/null; then
    echo "[信息] JsonCpp 库已安装"
    pkg-config --modversion jsoncpp
else
    echo "[信息] 正在安装 JsonCpp 库..."
    case $OS in
        ubuntu|debian)
            sudo apt-get install -y libjsoncpp-dev
            ;;
        centos|rhel|fedora)
            sudo yum install -y jsoncpp-devel
            ;;
        arch)
            sudo pacman -S jsoncpp
            ;;
        *)
            echo "[错误] 不支持的操作系统: $OS"
            echo "请手动安装 JsonCpp 库"
            exit 1
            ;;
    esac
    
    if [ $? -eq 0 ]; then
        echo "[成功] JsonCpp 库安装成功"
        pkg-config --modversion jsoncpp
    else
        echo "[错误] JsonCpp 库安装失败"
        exit 1
    fi
fi

echo

# 安装 pthread 库（通常已包含在 glibc 中）
echo "===================================="
echo "第五步：检查 pthread 库"
echo "===================================="
echo

if ldconfig -p | grep -q libpthread.so; then
    echo "[信息] pthread 库已安装"
else
    echo "[警告] 未找到 pthread 库"
    echo "pthread 通常包含在 glibc 中，如果编译失败请手动安装"
fi

echo

echo "===================================="
echo "[成功] 所有依赖安装完成！"
echo "===================================="
echo
echo "下一步："
echo "  1. 运行 ./build_linux.sh 编译项目"
echo "  2. 运行 ./parser 解析数据"
echo "  3. 运行 ./http_server 启动服务器"
echo "  4. 在浏览器访问 http://localhost:8080"
echo
echo "验证安装："
echo "  1. 验证 CMake: cmake --version"
echo "  2. 验证 g++: g++ --version"
echo "  3. 验证 Boost: pkg-config --modversion libboost_system"
echo "  4. 验证 JsonCpp: pkg-config --modversion jsoncpp"
echo
