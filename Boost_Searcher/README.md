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
└── CMakeLists.txt         # 构建配置
```

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

2. **倒排索引**
   - 使用哈希表存储关键字到文档列表的映射
   - 一个关键字可能对应多个文档
   - 支持权重计算，用于搜索结果排序

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

### 3. HTML 解析析模块 (parser.cpp)

#### 功能描述
解析模块负责从 Boost 官方文档中提取 HTML 文件，解析标题、内容和 URL，并保存为结构化的数据格式。

#### 主要功能

1. **文件枚举**
   - 递归遍历指定目录下的所有 HTML 文件
   - 过滤非 HTML 文件

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
    static cppjieba::Jieba& jieba(
#ifdef DICT_PATH
        DICT_PATH "/cppjieba/dict/jieba.dict.utf8",
        DICT_PATH "/cppjieba/dict/hmm_model.utf8",
        DICT_PATH "/cppjieba/dict/user.dict.utf8",
        DICT_PATH "/cppjieba/dict/idf.utf8",
        DICT_PATH "/cppjieba/dict/stop_words.utf8"
#else
        "./cppjieba/dict/jieba.dict.utf8",
        "./cppjieba/dict/hmm_model.utf8",
        "./cppjieba/dict/user.dict.utf8",
        "./cppjieba/dict/idf.utf8",
        "./cppjieba/dict/stop_words.utf8"
#endif
    );
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

## 编译和运行

### 编译步骤

#### Windows 平台

```bash
# 安装依赖
winget install Kitware.CMake
winget install Microsoft.VisualStudio.2022.BuildTools

# 安装 vcpkg
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat

# 安装依赖库
vcpkg install boost-system:x64-windows boost-filesystem:x64-windows
vcpkg install jsoncpp:x64-windows

# 编译项目
cmake -B build -DCMAKE_TOOLCHAIN_FILE="C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake"
cmake --build build --config Release
```

### 运行步骤

#### 1. 解析 HTML 文件

```bash
.\build\Release\parser.exe
```

#### 2. 启动 HTTP 服务器

```bash
.\build\Release\http_server.exe
```

#### 3. 访问搜索页面

在浏览器中打开：`http://localhost:8080`

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
- **vcpkg 包管理**：简化依赖安装
- **UTF-8 编码支持**：正确处理中文内容

## 依赖库

### 1. Boost 库

- **boost::filesystem**：文件系统操作
- **boost::algorithm**：字符串处理和算法

### 2. 第三方库

- **cppjieba**：中文分词库
- **cpp-httplib**：HTTP 服务器库
- **jsoncpp**：JSON 格式化输出

## 常见问题

### 1. 编译问题

**问题**：找不到 boost/algorithm/string.hpp 头文件

**解决方案**：
- 确保已正确安装 Boost 库
- 检查 CMakeLists.txt 中的依赖配置

### 2. 运行问题

**问题**：搜索按钮无效

**解决方案**：
- 检查数据文件是否正确解析
- 确认索引已正确构建
- 检查 HTTP 服务器是否正常运行
- 查看浏览器控制台是否有错误信息

### 3. 性能优化

**问题**：搜索响应慢

**解决方案**：
- 优化索引结构
- 增加缓存机制
- 使用更高效的分词算法

## 总结

Boost 搜索引擎是一个功能完整、设计合理的轻量级搜索引擎项目。通过模块化的架构设计，实现了从 HTML 解析到搜索服务的完整流程。项目采用 C++ 标准库和 Boost 库，保证了代码的高性能和可维护性。

## 快速开始

```bash
# 1. 解析 HTML 文件
.\build\Release\parser.exe

# 2. 启动 HTTP 服务器
.\build\Release\http_server.exe

# 3. 访问搜索页面
# 在浏览器中打开 http://localhost:8080
```

## 联系方式

如有问题或建议，请通过以下方式联系：

- 提交 Issue
- 发送邮件
- 参与项目讨论

---

**文档版本**：1.0  
**最后更新**：2025-01-24
