# Boost 搜索引擎 - 项目文档

## 项目概述

Boost 搜索引擎是一个基于 C++ 开发的轻量级搜索引擎，使用倒排索引技术实现高效的文本搜索功能。项目采用模块化设计，包含 HTML 解析、索引构建、搜索服务和 HTTP 服务器等核心模块。

## 项目架构

```
Boost_Searcher/
├── data/                    # 数据目录
│   ├── input/             # 原始 HTML 文件
│   └── raw_html/          # 解析后的数据文件
├── wwwroot/               # Web 资源目录
├── cppjieba/             # 中文分词库
├── cpp-httplib/          # HTTP 服务器库
├── index.hpp              # 索引模块
├── log.hpp                # 日志模块
├── parser.cpp             # HTML 解析析模块
├── searcher.hpp           # 搜索模块
├── http_server.cpp         # HTTP 服务器模块
├── util.hpp               # 工具模块
├── debug.cpp              # 调试模块
├── CMakeLists.txt         # 构建配置
├── build_linux.sh         # Linux 构建脚本
├── install_linux.sh        # Linux 依赖安装脚本
├── start_linux.sh         # Linux 快速启动脚本
├── Makefile              # 传统 Makefile 构建文件
└── LINUX_README.md        # Linux 版本详细文档
```

---

## 快速开始

### Windows 平台

#### 使用自动化脚本

```cmd
# 1. 安装依赖
install_dependencies.bat

# 2. 编译项目
build_windows.bat

# 3. 解析 HTML 文件
.\build\Release\parser.exe

# 4. 启动 HTTP 服务器
.\build\Release\http_server.exe

# 5. 访问搜索页面
# 在浏览器中打开 http://localhost:8080
```

### Linux 平台

#### 使用自动化脚本（推荐）

```bash
# 1. 安装依赖
chmod +x install_linux.sh
./install_linux.sh

# 2. 编译项目
chmod +x build_linux.sh
./build_linux.sh

# 3. 解析 HTML 文件
./parser

# 4. 启动 HTTP 服务器
./http_server

# 5. 访问搜索页面
# 在浏览器中打开 http://localhost:8080
```

#### 使用 Makefile

```bash
# 1. 编译项目
make clean
make

# 2. 解析 HTML 文件
./parser

# 3. 启动 HTTP 服务器
./http_server

# 4. 访问搜索页面
# 在浏览器中打开 http://localhost:8080
```

---

## Windows 平台详细说明

### 系统要求

- **操作系统**：Windows 10/11
- **编译器**：Visual Studio 2019/2022 或 MinGW
- **内存**：2GB RAM（推荐 4GB+）
- **磁盘空间**：500MB 可用空间

### 编译步骤

#### 方法一：使用自动化脚本（推荐）

```cmd
# 安装依赖
install_dependencies.bat

# 编译项目
build_windows.bat
```

#### 方法二：手动编译

```cmd
# 1. 安装 CMake
winget install Kitware.CMake

# 2. 安装 Visual Studio Build Tools
winget install Microsoft.VisualStudio.2022.BuildTools

# 3. 安装 vcpkg
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat

# 4. 安装依赖库
vcpkg install boost-system:x64-windows boost-filesystem:x64-windows
vcpkg install jsoncpp:x64-windows

# 5. 集成 vcpkg 到 CMake
vcpkg integrate install

# 6. 编译项目
cmake -B build -DCMAKE_TOOLCHAIN_FILE="C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake"
cmake --build build --config Release
```

### 运行步骤

#### 1. 解析 HTML 文件

```cmd
.\build\Release\parser.exe
```

**输出示例：**
```
[NORMAL][1769251416][获取index单例成功....][index.hpp : 38]
[NORMAL][1769251416][建立正排和倒排索引成功....][searcher.hpp : 42]
[NORMAL][1769251416][当前的已经建立的索引文档 : 50][index.hpp : 128]
[NORMAL][1769251416][当前的已经建立的索引文档 : 100][index.hpp : 128]
...
[NORMAL][1769251416][当前的已经建立的索引文档 : 8550][index.hpp : 128]
```

#### 2. 启动 HTTP 服务器

```cmd
.\build\Release\http_server.exe
```

**输出示例：**
```
[NORMAL][1769251416][获取index单例成功....][index.hpp : 38]
[NORMAL][1769251416][建立正排和倒排索引成功....][searcher.hpp : 42]
[NORMAL][1769251416][服务器启动成功......][http_server.cpp : 40]
```

#### 3. 访问搜索页面

在浏览器中打开：`http://localhost:8080`

**默认端口：** 8080  
**监听地址：** 0.0.0.0（所有网络接口）

---

## Linux 平台详细说明

### 系统要求

#### 最低配置
- **操作系统**：Ubuntu 18.04+ / Debian 10+ / CentOS 7+ / Fedora 30+
- **编译器**：GCC 7.0+ 或 Clang 5.0+
- **内存**：2GB RAM（推荐 4GB+）
- **磁盘空间**：500MB 可用空间

#### 推荐配置
- **操作系统**：Ubuntu 20.04+ / Debian 11+ / CentOS 8+ / Fedora 35+
- **编译器**：GCC 9.0+ 或 Clang 10.0+
- **内存**：4GB RAM
- **磁盘空间**：1GB 可用空间

### 依赖安装

#### Ubuntu/Debian 系统

```bash
# 更新软件包列表
sudo apt-get update

# 安装 CMake
sudo apt-get install -y cmake

# 安装编译工具
sudo apt-get install -y build-essential g++

# 安装 Boost 库
sudo apt-get install -y libboost-all-dev

# 安装 JsonCpp 库
sudo apt-get install -y libjsoncpp-dev

# 验证安装
cmake --version
g++ --version
pkg-config --modversion libboost_system
pkg-config --modversion jsoncpp
```

#### CentOS/RHEL 系统

```bash
# 更新软件包列表
sudo yum update

# 安装 CMake
sudo yum install -y cmake

# 安装编译工具
sudo yum groupinstall -y "Development Tools"
sudo yum install -y gcc-c++

# 安装 Boost 库
sudo yum install -y boost-devel

# 安装 JsonCpp 库
sudo yum install -y jsoncpp-devel

# 验证安装
cmake --version
g++ --version
pkg-config --modversion libboost_system
pkg-config --modversion jsoncpp
```

#### Fedora 系统

```bash
# 更新软件包列表
sudo dnf update

# 安装 CMake
sudo dnf install -y cmake

# 安装编译工具
sudo dnf install -y gcc-c++

# 安装 Boost 库
sudo dnf install -y boost-devel

# 安装 JsonCpp 库
sudo dnf install -y jsoncpp-devel

# 验证安装
cmake --version
g++ --version
pkg-config --modversion libboost_system
pkg-config --modversion jsoncpp
```

#### Arch Linux 系统

```bash
# 更新软件包列表
sudo pacman -Syu

# 安装 CMake
sudo pacman -S cmake

# 安装编译工具
sudo pacman -S base-devel gcc

# 安装 Boost 库
sudo pacman -S boost

# 安装 JsonCpp 库
sudo pacman -S jsoncpp

# 验证安装
cmake --version
g++ --version
pkg-config --modversion libboost_system
pkg-config --modversion jsoncpp
```

### 编译项目

#### 使用 CMake

```bash
# 创建构建目录
mkdir -p build

# 配置项目
cmake -B build -DCMAKE_BUILD_TYPE=Release

# 编译项目
cmake --build build --config Release

# 或者使用多线程编译
cmake --build build --config Release -j$(nproc)
```

#### 使用 Makefile

```bash
# 清理之前的编译文件
make clean

# 编译所有目标
make

# 或者只编译特定目标
make parser
make debug
make http_server
make test_jieba
```

### 运行项目

#### 1. 解析 HTML 文件

```bash
./parser
```

**输出示例：**
```
[NORMAL][1769256175][获取index单例成功....][index.hpp : 38]
[NORMAL][1769256175][当前的已经建立的索引文档 : 50][index.hpp : 128]
[NORMAL][1769256175][当前的已经建立的索引文档 : 100][index.hpp : 128]
...
[NORMAL][1769256175][当前的已经建立的索引文档 : 8550][index.hpp : 128]
```

#### 2. 启动 HTTP 服务器

```bash
./http_server
```

**输出示例：**
```
[NORMAL][1769256175][获取index单例成功....][index.hpp : 38]
[NORMAL][1769256175][建立正排和倒排索引成功....][searcher.hpp : 42]
[NORMAL][1769256175][服务器启动成功......][http_server.cpp : 40]
```

#### 3. 访问搜索页面

在浏览器中打开：`http://localhost:8080`

**默认端口：** 8080  
**监听地址：** 0.0.0.0（所有网络接口）

### 测试搜索功能

```bash
# 测试英文搜索
curl -s 'http://localhost:8080/s?word=boost'

# 测试中文搜索
curl -s 'http://localhost:8080/s?word=搜索引擎'
```

---

## 核心模块介绍

### 1. 索引模块 (index.hpp)

#### 功能描述
索引模块负责构建和管理搜索引擎的正排索引和倒排索引，提供高效的文档检索功能。

#### 核心数据结构

```cpp
struct DocInfo {
    std::string title;    // 文档的标题
    std::string content; // 文档的内容（去标签后）
    std::string url;      // 文档在官网当中的 URL
    uint64_t doc_id;      // 文档的唯一标识符
};

struct InvertedElem {
    uint64_t doc_id;    // 文档 ID
    int weight;           // 重复文档的权重之和
    std::vector<std::string> words;  // 关键字的集合
};

class Index {
private:
    std::vector<DocInfo> forward_index;      // 正排索引（数组实现）
    std::unordered_map<std::string, InvertedList> inverted_index;  // 倒排索引（哈希表实现）
    std::mutex mtx;  // 互斥锁，保证线程安全
    static Index* instance;  // 单例模式
public:
    static Index* GetInstance();  // 获取单例
    bool BuildIndex(const std::string& input);  // 构建索引
    DocInfo* GetForwardIndex(uint64_t doc_id);  // 根据文档 ID 获取文档信息
    InvertedList* GetInvertedList(const std::string& word);  // 根据关键字获取倒排链
};
```

#### 主要功能

1. **正排索引**
   - 使用数组存储文档信息，数组下标天然对应文档 ID
   - 支持快速根据文档 ID 查询文档内容
   - 时间复杂度：O(1)

2. **倒排索引**
   - 使用哈希表存储关键字到文档列表的映射
   - 一个关键字可能对应多个文档
   - 支持权重计算，用于搜索结果排序
   - 平均时间复杂度：O(1)

3. **线程安全**
   - 使用互斥锁保护单例模式
   - 支持多线程环境下的安全访问

4. **单例模式**
   - 确保整个程序运行期间只有一个索引实例
   - 节省内存资源

#### 使用示例

```cpp
#include "index.hpp"

int main() {
    ns_index::Index* index = ns_index::Index::GetInstance();
    
    index->BuildIndex("data/raw_html/raw.txt");
    
    ns_index::DocInfo* doc = index->GetForwardIndex(0);
    if (doc != nullptr) {
        std::cout << "Title: " << doc->title << std::endl;
        std::cout << "URL: " << doc->url << std::endl;
    }
    
    ns_index::InvertedList* list = index->GetInvertedList("boost");
    if (list != nullptr) {
        std::cout << "Found " << list->size() << " documents" << std::endl;
    }
    
    return 0;
}
```

---

### 2. 日志模块 (log.hpp)

#### 功能描述
日志模块提供统一的日志输出功能，支持不同级别的日志记录，便于调试和问题排查。

#### 日志级别

```cpp
#define NORMAL   1  // 正常信息
#define WARNING  2  // 警告信息
#define DEBUG    3  // 调试信息
#define FATAL    4  // 致命错误
```

#### 主要功能

1. **分级日志**
   - 支持四种日志级别：NORMAL、WARNING、DEBUG、FATAL
   - 不同级别使用不同的输出格式

2. **详细信息**
   - 包含时间戳
   - 包含日志级别
   - 包含日志消息
   - 包含文件名和行号

3. **宏定义**
   - 使用宏简化日志调用
   - 支持条件编译

#### 使用示例

```cpp
#include "log.hpp"

int main() {
    LOG(NORMAL, "程序启动");
    LOG(DEBUG, "调试信息：变量值 = " << value);
    LOG(WARNING, "警告：文件不存在");
    LOG(FATAL, "致命错误：无法打开文件");
    
    return 0;
}
```

---

### 3. HTML 解析析模块 (parser.cpp)

#### 功能描述
解析模块负责从 Boost 官方文档中提取 HTML 文件，解析标题、内容和 URL，并保存为结构化的数据格式。

#### 主要功能

1. **文件枚举**
   - 递归遍历指定目录下的所有 HTML 文件
   - 过滤非 HTML 文件
   - 使用 Boost.Filesystem 库

2. **HTML 解析**
   - 提取 `<title>` 标签内容
   - 去除 HTML 标签，获取纯文本内容
   - 构建文档 URL

3. **数据保存**
   - 将解析后的数据保存到指定文件
   - 使用 `\3` 作为分隔符

#### 核心函数

```cpp
bool EnumFile(const std::string& src_path, std::vector<std::string>* files_list);
bool ParseHtml(const std::vector<std::string>& files_list, std::vector<DocInfo_t>* results);
bool SaveHtml(const std::vector<DocInfo_t>& results, const std::string& output);
```

#### 使用示例

```cpp
#include "parser.cpp"

int main() {
    std::vector<DocInfo_t> results;
    
    if (!EnumFile("data/input", &files_list)) {
        std::cerr << "枚举文件失败" << std::endl;
        return 1;
    }
    
    if (!ParseHtml(files_list, &results)) {
        std::cerr << "解析 HTML 失败" << std::endl;
        return 2;
    }
    
    if (!SaveHtml(results, "data/raw_html/raw.txt")) {
        std::cerr << "保存数据失败" << std::endl;
        return 3;
    }
    
    std::cout << "解析完成，共处理 " << results.size() << " 个文件" << std::endl;
    return 0;
}
```

---

### 4. 搜索模块 (searcher.hpp)

#### 功能描述
搜索模块负责处理用户搜索请求，实现关键词分词、索引查询、结果排序和 JSON 格式化输出。

#### 核心功能

1. **关键词分词**
   - 使用 cppjieba 库进行中文分词
   - 支持搜索模式分词

2. **索引查询**
   - 查询正排索引获取文档信息
   - 查询倒排索引获取相关文档列表

3. **结果合并**
   - 合并重复文档，累加权重
   - 去除重复结果

4. **结果排序**
   - 根据权重降序排序
   - 返回最相关的搜索结果

5. **JSON 格式化**
   - 生成符合标准的 JSON 格式输出
   - 包含标题、描述、URL 等信息

#### 主要函数

```cpp
void InitSearcher(const std::string& input);
void Search(const std::string& query, std::string* json_string);
std::string GetDesc(const std::string& html_content, const std::string& word);
```

#### 使用示例

```cpp
#include "searcher.hpp"

int main() {
    ns_searcher::Searcher searcher;
    searcher.InitSearcher("data/raw_html/raw.txt");
    
    std::string json_string;
    searcher.Search("boost", &json_string);
    
    std::cout << json_string << std::endl;
    return 0;
}
```

---

### 5. HTTP 服务器模块 (http_server.cpp)

#### 功能描述
HTTP 服务器模块提供 Web 服务接口，处理搜索请求并返回 JSON 格式的搜索结果。

#### 核心功能

1. **HTTP 服务器**
   - 使用 cpp-httplib 库实现 HTTP 服务器
   - 监听指定端口（默认 8080）
   - 支持静态文件服务

2. **请求处理**
   - 处理 GET 请求
   - 解析查询参数
   - 调用搜索模块

3. **响应格式**
   - 返回 JSON 格式的搜索结果
   - 支持 UTF-8 编码

#### 主要函数

```cpp
int main() {
    ns_searcher::Searcher search;
    search.InitSearcher("data/raw_html/raw.txt");
    
    httplib::Server svr;
    svr.set_base_dir("./wwwroot");
    
    svr.Get("/s", [&search](const httplib::Request& req, httplib::Response& rsp) {
        if (!req.has_param("word")) {
            rsp.set_content("必须要有搜索关键字!", "text/plain; charset=utf-8");
            return;
        }
        
        std::string word = req.get_param_value("word");
        std::string json_string;
        search.Search(word, &json_string);
        
        rsp.set_content(json_string, "application/json");
    });
    
    svr.listen("0.0.0.0", 8080);
    return 0;
}
```

---

### 6. 工具模块 (util.hpp)

#### 功能描述
工具模块提供文件操作、字符串处理和中文分词等通用工具函数。

#### 主要功能

1. **文件操作**
   - 读取文件内容
   - 写入文件内容

2. **字符串处理**
   - 字符串分割
   - 字符串转换

3. **中文分词**
   - 集成 cppjieba 分词库
   - 支持搜索模式分词

#### 主要类

```cpp
class FileUtil {
    static bool ReadFile(const std::string& file_path, std::string* out);
};

class StringUtil {
    static void Split(const std::string& target, std::vector<std::string>* out, const std::string& sep);
};

class JiebaUtil {
private:
    static cppjieba::Jieba jieba;
public:
    static void CutString(const std::string& src, std::vector<std::string>* out);
};
```

#### 使用示例

```cpp
#include "util.hpp"

int main() {
    std::string content;
    ns_util::FileUtil::ReadFile("data/input/test.html", &content);
    
    std::vector<std::string> words;
    ns_util::JiebaUtil::CutString(content, &words);
    
    for (const auto& word : words) {
        std::cout << word << std::endl;
    }
    
    return 0;
}
```

---

### 7. 调试模块 (debug.cpp)

#### 功能描述
调试模块提供命令行交互界面，用于测试搜索功能。

#### 主要功能

1. **交互式搜索**
   - 命令行输入搜索关键词
   - 显示搜索结果

2. **结果展示**
   - JSON 格式化输出
   - 支持持续搜索

#### 使用示例

```cpp
#include "debug.cpp"

int main() {
    ns_searcher::Searcher* search = new ns_searcher::Searcher();
    search->InitSearcher("data/raw_html/raw.txt");
    
    std::string query;
    std::string json_string;
    char buffer[1024];
    
    while (true) {
        std::cout << "Please Enter You Search Query：";
        fgets(buffer, sizeof(buffer) - 1, stdin);
        buffer[strlen(buffer) - 1] = 0;
        query = buffer;
        
        search->Search(query, &json_string);
        std::cout << json_string << std::endl;
    }
    
    delete search;
    return 0;
}
```

---

## 数据流程

### 1. 数据准备

```
Boost 官方文档 (HTML)
    ↓
HTML 解析 (parser.cpp)
    ↓
结构化数据 (raw.txt)
```

### 2. 索引构建

```
结构化数据 (raw.txt)
    ↓
索引构建 (index.hpp)
    ↓
正排索引 + 倒排索引
```

### 3. 搜索服务

```
用户搜索请求
    ↓
关键词分词 (cppjieba)
    ↓
索引查询
    ↓
结果合并与排序
    ↓
JSON 格式化输出
```

---

## 技术特点

### 1. 高性能

- **数组实现正排索引**：O(1) 时间复杂度查询文档信息
- **哈希表实现倒排索引**：O(1) 平均时间复杂度查询倒排链
- **单例模式**：减少内存占用，保证线程安全

### 2. 模块化设计

- **清晰的职责分离**：每个模块专注于特定功能
- **易于维护**：模块间通过接口通信
- **可扩展性**：支持添加新的搜索算法

### 3. 跨平台支持

- **CMake 构建系统**：支持 Windows、Linux 等多平台
- **标准 C++ 库**：保证代码的可移植性
- **UTF-8 编码支持**：正确处理中文内容

---

## 依赖库

### 1. Boost 库

- **boost::filesystem**：文件系统操作
- **boost::algorithm**：字符串处理和算法

### 2. 第三方库

- **cppjieba**：中文分词库
- **cpp-httplib**：HTTP 服务器库
- **jsoncpp**：JSON 格式化输出

### 3. 系统库

- **pthread**：多线程支持（Linux）
- **Windows API**：多线程支持（Windows）

---

## 常见问题

### 1. 编译问题

#### Windows 平台

**问题**：找不到 boost/algorithm/string.hpp 头文件

**解决方案**：
- 确保已正确安装 Boost 库
- 检查 CMakeLists.txt 中的依赖配置
- 验证 Boost 库路径是否正确

#### Linux 平台

**问题**：找不到 Boost 库

**解决方案**：
```bash
# Ubuntu/Debian
sudo apt-get install libboost-all-dev

# CentOS/RHEL
sudo yum install boost-devel

# Fedora
sudo dnf install boost-devel

# Arch Linux
sudo pacman -S boost
```

**问题**：找不到 JsonCpp 库

**解决方案**：
```bash
# Ubuntu/Debian
sudo apt-get install libjsoncpp-dev

# CentOS/RHEL
sudo yum install jsoncpp-devel

# Fedora
sudo dnf install jsoncpp-devel

# Arch Linux
sudo pacman -S jsoncpp
```

### 2. 运行问题

#### Windows 平台

**问题**：搜索按钮无效

**解决方案**：
- 检查数据文件是否正确解析
- 确认索引已正确构建
- 检查 HTTP 服务器是否正常运行
- 查看浏览器控制台是否有错误信息

#### Linux 平台

**问题**：搜索按钮无效

**解决方案**：
- 检查数据文件是否正确解析
- 确认索引已正确构建
- 检查 HTTP 服务器是否正常运行
- 查看浏览器控制台是否有错误信息

**问题**：端口被占用

**解决方案**：
```bash
# 查看端口占用情况
netstat -tuln | grep 8080
# 或
lsof -i :8080

# 修改端口
# 编辑 http_server.cpp，将 8080 改为其他端口
svr.listen("0.0.0.0", 8081);
```

### 3. 性能优化

#### Windows 平台

**问题**：搜索响应慢

**解决方案**：
- 优化索引结构
- 增加缓存机制
- 使用更高效的分词算法
- 增加服务器资源

#### Linux 平台

**问题**：搜索响应慢

**解决方案**：
- 优化索引结构
- 增加缓存机制
- 使用更高效的分词算法
- 增加服务器资源

---

## 系统服务配置

### Linux 平台

#### 使用 systemd 管理服务

创建服务文件 `/etc/systemd/system/boost-searcher.service`：

```ini
[Unit]
Description=Boost Search Engine Service
After=network.target

[Service]
Type=simple
User=your_username
WorkingDirectory=/path/to/Boost_Searcher
ExecStart=/path/to/Boost_Searcher/http_server
Restart=on-failure
RestartSec=10

[Install]
WantedBy=multi-user.target
```

启动服务：

```bash
sudo systemctl daemon-reload
sudo systemctl enable boost-searcher
sudo systemctl start boost-searcher
sudo systemctl status boost-searcher
```

#### 使用 Docker 部署

创建 Dockerfile：

```dockerfile
FROM ubuntu:20.04

# 安装依赖
RUN apt-get update && apt-get install -y \
    cmake \
    g++ \
    libboost-all-dev \
    libjsoncpp-dev

# 复制项目文件
COPY . /app
WORKDIR /app

# 编译项目
RUN cmake -B build && cmake --build build

# 暴露端口
EXPOSE 8080

# 启动服务
CMD ["./http_server"]
```

构建和运行：

```bash
# 构建镜像
docker build -t boost-searcher .

# 运行容器
docker run -p 8080:8080 boost-searcher
```

---

## 监控和日志

### Linux 平台

#### 日志输出

程序使用标准输出输出日志，可以重定向到文件：

```bash
# 重定向到文件
./http_server > server.log 2>&1

# 使用 tee 同时输出到屏幕和文件
./http_server 2>&1 | tee server.log
```

#### 系统监控

```bash
# 监控 CPU 使用
top -p $(pgrep http_server)

# 监控内存使用
ps aux | grep http_server

# 监控网络连接
netstat -tuln | grep 8080
```

---

## 安全配置

### Linux 平台

#### 防火墙配置

```bash
# Ubuntu/Debian (ufw)
sudo ufw allow 8080/tcp
sudo ufw reload

# CentOS/RHEL (firewalld)
sudo firewall-cmd --permanent --add-port=8080/tcp
sudo firewall-cmd --reload

# Fedora
sudo firewall-cmd --permanent --add-port=8080/tcp
sudo firewall-cmd --reload
```

#### Nginx 反向代理

配置 Nginx 作为反向代理：

```nginx
server {
    listen 80;
    server_name search.example.com;

    location / {
        proxy_pass http://localhost:8080;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
        proxy_set_header X-Forwarded-Proto $scheme;
    }
}
```

---

## 总结

Boost 搜索引擎是一个功能完整、设计合理的轻量级搜索引擎项目。通过模块化的架构设计，实现了从 HTML 解析到搜索服务的完整流程。项目采用 C++ 标准库和 Boost 库，保证了代码的高性能和可维护性。

### 主要特性

- ✅ **跨平台支持**：Windows 和 Linux 双平台
- ✅ **模块化设计**：清晰的职责分离
- ✅ **高性能**：倒排索引技术
- ✅ **中文分词**：支持中文搜索
- ✅ **Web 服务**：HTTP 接口
- ✅ **完整文档**：详细的使用说明

### 平台差异

#### Windows 平台
- 使用 Visual Studio 编译
- 使用 vcpkg 管理依赖
- 支持 Windows API

#### Linux 平台
- 使用 GCC 编译
- 使用包管理器安装依赖
- 支持 POSIX 标准

---

## 联系方式

如有问题或建议，请通过以下方式联系：

- 提交 Issue
- 发送邮件
- 参与项目讨论

---

**文档版本**：2.0  
**最后更新**：2025-01-25  
**适用平台**：Windows、Linux (Ubuntu/Debian/CentOS/RHEL/Fedora/Arch)
