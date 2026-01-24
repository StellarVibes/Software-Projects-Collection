# Boost_Searcher Windows 版本使用说明

## 项目简介
这是一个基于 Boost 库的搜索引擎项目，使用 cppjieba 进行中文分词。已修改为在 Windows 平台上可以运行。

## 主要修改内容

### 1. 创建了 CMakeLists.txt 构建文件
- 替代了原来的 Makefile
- 支持 MSVC 和 MinGW 编译器
- 自动检测 Windows 平台并使用正确的库链接

### 2. 修复了 cppjieba 分词库路径问题
- 使用 CMake 定义 DICT_PATH 宏
- 支持相对路径和绝对路径
- 兼容 Windows 和 Linux 路径分隔符

### 3. 修复了文件路径分隔符问题
- 使用 Boost.Filesystem 库处理跨平台路径
- 创建了必要的目录结构（data/raw_html, wwwroot）

## 依赖库

### 必需的库：
1. **Boost** (版本 1.85.0 或更高)
   - 组件：system, filesystem
   - 下载地址：https://www.boost.org/users/download/

2. **JsonCpp**
   - 用于 JSON 数据处理
   - 下载地址：https://github.com/open-source-parsers/jsoncpp

3. **CMake** (版本 3.10 或更高)
   - 用于构建项目
   - 下载地址：https://cmake.org/download/

4. **Visual Studio** (MSVC 编译器)
   - 用于编译 C++ 代码

### 项目内置的库：
- **cppjieba** - 中文分词库（已包含在项目中）
- **cpp-httplib** - HTTP 服务器库（已包含在项目中）

## 编译步骤

### 方法一：使用批处理脚本（推荐）
1. 双击运行 `build_windows.bat`
2. 脚本会自动配置和编译项目

### 方法二：手动编译
1. 打开命令提示符或 PowerShell
2. 进入项目目录：
   ```cmd
   cd c:\Users\19890\Documents\trae_projects\trae-project\Boost_Searcher
   ```
3. 配置项目：
   ```cmd
   cmake -B build
   ```
4. 编译项目：
   ```cmd
   cmake --build build --config Release
   ```

## 运行步骤

### 1. 测试分词功能（可选）
```cmd
.\build\Release\test_jieba.exe
```
这将：
- 测试 cppjieba 分词库是否正常工作
- 显示几个测试句子的分词结果
- 帮助验证分词功能是否正确

### 2. 解析 HTML 数据
```cmd
.\build\Release\parser.exe
```
这将：
- 读取 `data/input` 目录下的所有 HTML 文件
- 解析并提取标题、内容和 URL
- 将结果保存到 `data/raw_html/raw.txt`

### 3. 启动 HTTP 服务器
```cmd
.\build\Release\http_server.exe
```
这将：
- 加载索引数据
- 启动 HTTP 服务器（端口 8080）
- 在浏览器中访问 http://localhost:8080

### 4. 调试模式（可选）
```cmd
.\build\Release\debug.exe
```
这将：
- 启动命令行搜索界面
- 输入搜索关键词进行测试

## 项目结构

```
Boost_Searcher/
├── CMakeLists.txt          # CMake 构建配置
├── build_windows.bat       # Windows 编译脚本
├── parser.cpp              # HTML 解析器
├── http_server.cpp         # HTTP 服务器
├── debug.cpp               # 调试程序
├── index.hpp               # 索引类定义
├── searcher.hpp            # 搜索器类定义
├── util.hpp                # 工具类（文件、字符串、分词）
├── log.hpp                 # 日志工具
├── cppjieba/               # 中文分词库
│   ├── dict/               # 分词词典
│   └── include/            # 头文件
├── cpp-httplib/            # HTTP 库
├── data/
│   ├── input/              # 输入 HTML 文件
│   └── raw_html/           # 解析后的数据
└── wwwroot/
    └── index.html          # 搜索页面
```

## 常见问题

### Q1: 编译时提示找不到 Boost 库
A: 确保 Boost 已正确安装，并设置环境变量 BOOST_ROOT 指向 Boost 安装目录。

### Q2: 编译时提示找不到 JsonCpp 库
A: 确保 JsonCpp 已正确安装，或在 CMakeLists.txt 中指定 JsonCpp 的路径。

### Q3: 运行时提示找不到词典文件
A: 确保 `cppjieba/dict/` 目录下有以下文件：
- jieba.dict.utf8
- hmm_model.utf8
- user.dict.utf8
- idf.utf8
- stop_words.utf8

### Q4: 分词功能不工作
A: 检查词典文件路径是否正确，确保所有词典文件都在 `cppjieba/dict/` 目录下。

## 技术说明

### 分词实现
项目使用 cppjieba 库进行中文分词，主要功能：
- CutForSearch: 搜索模式分词
- 支持自定义词典
- 支持停用词过滤

### 索引结构
- 正排索引：使用 vector 存储，数组下标作为文档 ID
- 倒排索引：使用 unordered_map 存储关键词到文档列表的映射
- 权重计算：标题权重 × 10 + 内容权重 × 1

### 搜索流程
1. 用户输入搜索关键词
2. 对关键词进行分词
3. 在倒排索引中查找相关文档
4. 按权重排序
5. 生成 JSON 结果返回给用户

## 许可证
本项目遵循原项目的许可证。
