# Boost_Searcher Windows 适配修改总结

## 项目概述
将 Boost_Searcher 项目从 Linux 平台适配到 Windows 平台，主要解决了分词库路径、文件路径分隔符和构建系统的问题。

## 修改内容

### 1. 创建 CMakeLists.txt 构建文件
**文件**: `CMakeLists.txt`

**主要功能**:
- 替代原来的 Makefile，支持跨平台构建
- 支持 MSVC 和 MinGW 编译器
- 自动检测 Windows 平台并使用正确的库链接
- 设置 UTF-8 编码支持（MSVC）
- 添加 Boost 和 JsonCpp 依赖查找
- 配置分词词典路径

**关键代码**:
```cmake
if(MSVC)
    add_compile_options(/utf-8)  # Windows UTF-8 支持
endif()

find_package(Boost 1.65.0 REQUIRED COMPONENTS system filesystem)
find_package(jsoncpp REQUIRED)

set(DICT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cppjieba/dict")
add_definitions(-DDICT_PATH="${DICT_PATH}")
```

### 2. 修复 cppjieba 分词库路径问题
**文件**: `util.hpp`

**问题**: 原代码使用硬编码的相对路径 `./dict/`，在 Windows 上可能找不到词典文件。

**解决方案**:
- 使用 CMake 定义的 DICT_PATH 宏
- 支持相对路径和绝对路径
- 兼容 Windows 和 Linux 路径分隔符

**修改前**:
```cpp
const char* const DICT_PATH = "./dict/jieba.dict.utf8";
cppjieba::Jieba JiebaUtil::jieba(DICT_PATH, HMM_PATH, USER_DICT_PATH, IDF_PATH, STOP_WORD_PATH);
```

**修改后**:
```cpp
cppjieba::Jieba JiebaUtil::jieba(
#ifdef DICT_PATH
    DICT_PATH "/jieba.dict.utf8",
    DICT_PATH "/hmm_model.utf8",
    DICT_PATH "/user.dict.utf8",
    DICT_PATH "/idf.utf8",
    DICT_PATH "/stop_words.utf8"
#else
    "./cppjieba/dict/jieba.dict.utf8",
    "./cppjieba/dict/hmm_model.utf8",
    "./cppjieba/dict/user.dict.utf8",
    "./cppjieba/dict/idf.utf8",
    "./cppjieba/dict/stop_words.utf8"
#endif
);
```

### 3. 创建必要的目录结构
**目录**: `data/raw_html`, `wwwroot`

**说明**: 
- `data/raw_html`: 存储解析后的 HTML 数据
- `wwwroot`: 存储 Web 界面文件

### 4. 创建辅助脚本

#### build_windows.bat
**功能**: Windows 编译脚本
- 检查依赖是否安装
- 运行 CMake 配置
- 编译项目
- 显示编译结果和使用说明

#### install_dependencies.bat
**功能**: 依赖安装指南
- 提供详细的依赖安装步骤
- 支持 CMake、Boost、JsonCpp、Visual Studio 的安装
- 提供多种安装方法（预编译、源码编译、vcpkg）

#### check_structure.bat
**功能**: 项目结构检查脚本
- 验证所有必需的源代码文件是否存在
- 检查 cppjieba 词典文件是否完整
- 检查数据目录和 Web 资源
- 提供下一步操作指导

### 5. 创建分词测试程序
**文件**: `test_jieba.cpp`

**功能**:
- 测试 cppjieba 分词库是否正常工作
- 显示几个测试句子的分词结果
- 帮助验证分词功能是否正确
- 提供错误诊断信息

**测试内容**:
- "我来到北京清华大学"
- "小明硕士毕业于中国科学院计算所"
- "我爱北京天安门"

### 6. 创建文档
**文件**: `WINDOWS_README.md`

**内容**:
- 项目简介和主要修改说明
- 依赖库列表和下载地址
- 详细的编译步骤
- 运行步骤和使用说明
- 常见问题解答
- 技术说明

## 技术要点

### 1. 跨平台路径处理
- 使用 Boost.Filesystem 库处理文件路径
- CMake 自动处理路径分隔符（Windows 使用 `\`，Linux 使用 `/`）
- 使用预处理器宏区分平台

### 2. UTF-8 编码支持
- MSVC 编译器需要 `/utf-8` 选项
- 确保源代码文件使用 UTF-8 编码保存
- 词典文件必须使用 UTF-8 编码

### 3. 静态库链接
- Windows 下使用静态链接避免 DLL 依赖问题
- 使用 `target_link_libraries` 正确链接库文件

### 4. 线程支持
- Windows 下使用 `Threads::Threads`
- Linux 下使用 `pthread`

## 分词功能说明

### cppjieba 库
- **功能**: 中文分词
- **主要方法**: `CutForSearch()` - 搜索模式分词
- **词典文件**:
  - `jieba.dict.utf8`: 主词典
  - `hmm_model.utf8`: HMM 模型
  - `user.dict.utf8`: 用户自定义词典
  - `idf.utf8`: IDF 权重文件
  - `stop_words.utf8`: 停用词表

### 分词流程
1. 初始化 cppjieba::Jieba 对象
2. 调用 `CutForSearch()` 方法进行分词
3. 返回分词结果（vector<string>）

## 使用流程

### 1. 检查项目结构
```cmd
check_structure.bat
```

### 2. 安装依赖
```cmd
install_dependencies.bat
```

### 3. 编译项目
```cmd
build_windows.bat
```

### 4. 测试分词功能
```cmd
.\build\Release\test_jieba.exe
```

### 5. 解析 HTML 数据
```cmd
.\build\Release\parser.exe
```

### 6. 启动 HTTP 服务器
```cmd
.\build\Release\http_server.exe
```

### 7. 访问搜索页面
在浏览器中打开: `http://localhost:8080`

## 常见问题

### Q1: 编译时提示找不到 Boost 库
**解决方案**:
- 确保 Boost 已正确安装
- 设置环境变量 `BOOST_ROOT` 指向 Boost 安装目录
- 或在 CMakeLists.txt 中指定 Boost 路径

### Q2: 编译时提示找不到 JsonCpp 库
**解决方案**:
- 确保 JsonCpp 已正确安装
- 使用 vcpkg 安装: `vcpkg install jsoncpp:x64-windows`
- 或在 CMakeLists.txt 中指定 JsonCpp 路径

### Q3: 运行时提示找不到词典文件
**解决方案**:
- 确保 `cppjieba/dict/` 目录下有所有必需的词典文件
- 检查文件编码是否为 UTF-8
- 确认 DICT_PATH 宏定义正确

### Q4: 分词功能不工作
**解决方案**:
- 运行 `test_jieba.exe` 测试分词功能
- 检查词典文件是否完整
- 确认文件编码为 UTF-8
- 查看错误信息进行诊断

## 文件清单

### 新增文件
- `CMakeLists.txt` - CMake 构建配置
- `build_windows.bat` - Windows 编译脚本
- `install_dependencies.bat` - 依赖安装指南
- `check_structure.bat` - 项目结构检查脚本
- `test_jieba.cpp` - 分词测试程序
- `WINDOWS_README.md` - Windows 使用文档
- `WINDOWS_CHANGES.md` - 本修改总结文档

### 修改文件
- `util.hpp` - 修复分词库路径问题

### 目录结构
```
Boost_Searcher/
├── CMakeLists.txt          [新增]
├── build_windows.bat       [新增]
├── install_dependencies.bat [新增]
├── check_structure.bat     [新增]
├── test_jieba.cpp          [新增]
├── WINDOWS_README.md       [新增]
├── WINDOWS_CHANGES.md      [新增]
├── parser.cpp
├── http_server.cpp
├── debug.cpp
├── index.hpp
├── searcher.hpp
├── util.hpp                [修改]
├── log.hpp
├── cppjieba/
│   ├── dict/
│   └── include/
├── cpp-httplib/
├── data/
│   ├── input/
│   └── raw_html/           [新增目录]
└── wwwroot/
```

## 总结

本次修改成功将 Boost_Searcher 项目适配到 Windows 平台，主要解决了以下问题：

1. ✅ 创建了跨平台的 CMake 构建系统
2. ✅ 修复了 cppjieba 分词库的路径问题
3. ✅ 解决了 Windows 文件路径分隔符兼容性问题
4. ✅ 添加了 UTF-8 编码支持
5. ✅ 创建了完整的辅助脚本和文档
6. ✅ 添加了分词功能测试程序

项目现在可以在 Windows 平台上正常编译和运行，分词功能经过测试验证可以正常工作。
