# ConcurrentMemoryPool 模块介绍和使用文档

## 项目概述

ConcurrentMemoryPool 是一个高并发内存池实现，基于 Google TCMalloc 的设计思路。该项目采用三级缓存架构（ThreadCache、CentralCache、PageCache），实现了高效的内存分配和释放，特别适用于多线程环境下的高频内存分配场景。

## 模块列表

1. [Common.h - 通用定义和基础工具类](#1-commonh)
2. [ThreadCache - 线程缓存](#2-threadcache)
3. [CentralCache - 中心缓存](#3-centralcache)
4. [PageCache - 页缓存](#4-pagecache)
5. [PageMap - 页映射（基数树）](#5-pagemap)
6. [ObjectPool - 对象池](#6-objectpool)
7. [ConcurrentAlloc - 并发分配接口](#7-concurrentalloc)

---

## 1. Common.h

### 模块简介

[Common.h](file:///d:/GitHub/Software-Projects-Collection/ConcurrentMemoryPool/Common.h) 是整个项目的核心基础模块，定义了内存池的全局常量、数据结构和工具类。

### 主要组件

#### 全局常量

```cpp
static const size_t MAX_BYTES = 256 * 1024;      // ThreadCache 最大分配 256KB
static const size_t NFREELISTS = 208;            // ThreadCache 和 CentralCache 的哈希桶数量
static const size_t NPAGES = 129;                // PageCache 的哈希桶数量
static const size_t PAGE_SHIFT = 13;             // 页大小偏移（一页大小为 2^13 = 8KB）
```

#### FreeList - 自由链表类

管理已释放对象的链表，支持单个和批量操作。

**主要方法：**

- `void Push(void* obj)` - 将对象插入链表头部
- `void* Pop()` - 从链表头部取出一个对象
- `void PushRange(void* start, void* end, size_t n)` - 批量插入对象
- `void PopRange(void*& start, void*& end, size_t n)` - 批量取出对象
- `bool Empty()` - 判断链表是否为空
- `size_t Size()` - 获取链表中的对象数量
- `size_t& MaxSize()` - 获取/设置最大容量

#### SizeClass - 大小映射类

负责将任意大小的内存请求映射到合适的哈希桶索引，并计算对齐后的内存大小。

**主要方法：**

- `static size_t RoundUp(size_t bytes)` - 计算对齐后的内存大小
- `static size_t Index(size_t bytes)` - 获取对应的哈希桶索引
- `static size_t NumMoveSize(size_t size)` - 计算 ThreadCache 从 CentralCache 获取的对象数量
- `static size_t NumMovePage(size_t size)` - 计算 CentralCache 从 PageCache 获取的页数

**内存对齐规则：**

| 字节范围 | 对齐大小 | 哈希桶索引范围 |
|---------|---------|---------------|
| [1, 128] | 8 bytes | [0, 16) |
| [129, 1024] | 16 bytes | [16, 72) |
| [1025, 8192] | 128 bytes | [72, 128) |
| [8193, 65536] | 1024 bytes | [128, 184) |
| [65537, 262144] | 8192 bytes | [184, 208) |

#### Span - 跨页内存块结构

表示以页为单位的大块内存。

**成员变量：**

```cpp
PAGE_ID _pageId;      // 起始页号
size_t _n;            // 页数量
Span* _next;          // 双向链表指针
Span* _prev;
size_t _objSize;      // 切割成的小对象大小
size_t _useCount;     // 分配给 ThreadCache 的计数
void* _freeList;      // 切割成的小对象自由链表
bool _isUse;          // 是否正在使用
```

#### SpanList - 跨页链表类

带头结点的双向循环链表，用于管理 Span 对象。

**主要方法：**

- `Span* Begin()` - 获取第一个节点
- `Span* End()` - 获取头结点
- `bool Empty()` - 判断链表是否为空
- `void PushFront(Span* span)` - 头插
- `Span* PopFront()` - 头删
- `void Insert(Span* pos, Span* newSpan)` - 在指定位置插入
- `void Erase(Span* pos)` - 删除指定节点

### 系统内存接口

- `SystemAlloc(size_t kpage)` - 向系统申请 kpage 页内存
- `SystemFree(void* ptr)` - 向系统释放内存

---

## 2. ThreadCache

### 模块简介

[ThreadCache](file:///d:/GitHub/Software-Projects-Collection/ConcurrentMemoryPool/ThreadCache.h) 是每个线程独有的缓存，用于快速分配和释放小对象。通过 TLS（Thread Local Storage）机制，每个线程拥有独立的 ThreadCache 实例，避免了多线程竞争。

### 核心特性

- **线程私有**：每个线程独立拥有，无锁竞争
- **快速分配**：直接从自由链表获取对象
- **批量获取**：从 CentralCache 批量获取对象，减少锁竞争
- **批量回收**：当自由链表过长时，批量归还给 CentralCache

### 主要方法

#### void* Allocate(size_t size)

分配指定大小的内存。

**工作流程：**

1. 计算对齐后的内存大小和对应的哈希桶索引
2. 检查对应哈希桶的自由链表是否为空
3. 若不为空，直接从链表头部取出一个对象返回
4. 若为空，调用 `FetchFromCentralCache` 从 CentralCache 批量获取对象

#### void Deallocate(void* ptr, size_t size)

释放指定大小的内存。

**工作流程：**

1. 将对象插入对应哈希桶的自由链表
2. 检查链表长度是否超过最大容量
3. 若超过，调用 `ListTooLong` 批量归还给 CentralCache

#### void* FetchFromCentralCache(size_t index, size_t size)

从 CentralCache 批量获取对象。

**批量策略：**

- 采用慢启动算法，初始批量数量为 1
- 每次成功获取后，批量数量加 1
- 最大批量数量由 `SizeClass::NumMoveSize(size)` 决定（2~512 个）

#### void ListTooLong(FreeList& list, size_t size)

当自由链表过长时，批量归还对象给 CentralCache。

### 使用示例

```cpp
#include "ConcurrentAlloc.h"

void ThreadExample() {
    // 分配内存
    void* ptr1 = ConcurrentAlloc(100);
    void* ptr2 = ConcurrentAlloc(256);

    // 释放内存
    ConcurrentFree(ptr1);
    ConcurrentFree(ptr2);
}
```

---

## 3. CentralCache

### 模块简介

[CentralCache](file:///d:/GitHub/Software-Projects-Collection/ConcurrentMemoryPool/CentralCache.h) 是所有线程共享的中心缓存，负责在 ThreadCache 和 PageCache 之间进行内存的中转。采用单例模式，全局唯一实例。

### 核心特性

- **单例模式**：全局唯一实例
- **哈希桶锁**：每个哈希桶有独立的互斥锁，减少锁竞争
- **Span 管理**：管理多个 Span，每个 Span 切割成固定大小的对象
- **动态平衡**：根据 ThreadCache 的需求动态调整 Span 的分配

### 主要方法

#### size_t FetchRangeObj(void*& start, void*& end, size_t n, size_t size)

从指定哈希桶中获取 n 个对象。

**工作流程：**

1. 获取对应哈希桶的 Span
2. 从 Span 的自由链表中取出 n 个对象
3. 更新 Span 的使用计数
4. 返回实际获取的对象数量

#### Span* GetOneSpan(SpanList& spanList, size_t size)

获取一个非空的 Span。

**工作流程：**

1. 遍历当前哈希桶的 SpanList，寻找有自由对象的 Span
2. 若找到，直接返回
3. 若未找到，从 PageCache 申请新的 Span
4. 将新申请的 Span 切割成指定大小的对象
5. 将 Span 插入到哈希桶中

#### void ReleaseListToSpans(void* start, size_t size)

将一批对象归还给对应的 Span。

**工作流程：**

1. 遍历对象链表
2. 通过 `MapObjectToSpan` 找到对应的 Span
3. 将对象插入 Span 的自由链表
4. 更新 Span 的使用计数
5. 若 Span 的使用计数为 0，将 Span 归还给 PageCache

### 使用示例

```cpp
// CentralCache 通常不直接使用，而是通过 ThreadCache 间接调用
// 以下为内部使用示例

size_t index = SizeClass::Index(size);
void* start = nullptr;
void* end = nullptr;
size_t n = 10;
size_t actualNum = CentralCache::GetInstance()->FetchRangeObj(start, end, n, size);
```

---

## 4. PageCache

### 模块简介

[PageCache](file:///d:/GitHub/Software-Projects-Collection/ConcurrentMemoryPool/PageCache.h) 是以页为单位的缓存，负责从系统申请大块内存并按页分配给 CentralCache。采用单例模式，全局唯一实例。

### 核心特性

- **单例模式**：全局唯一实例
- **页管理**：以页（8KB）为单位管理内存
- **Span 合并**：释放时合并相邻的空闲 Span，减少内存碎片
- **基数树映射**：使用基数树快速查找对象对应的 Span

### 主要方法

#### Span* NewSpan(size_t k)

获取一个 k 页的 Span。

**工作流程：**

1. 若 k > 128，直接向系统申请
2. 检查 k 页哈希桶是否有空闲 Span
3. 若有，直接返回
4. 若没有，查找更大的空闲 Span 并分割
5. 若没有更大的 Span，向系统申请 128 页的 Span 并分割
6. 建立页号到 Span 的映射关系

#### Span* MapObjectToSpan(void* obj)

通过对象地址查找对应的 Span。

**工作流程：**

1. 将对象地址转换为页号
2. 通过基数树查找页号对应的 Span
3. 返回 Span 指针

#### void ReleaseSpanToPageCache(Span* span)

将 Span 归还给 PageCache 并尝试合并。

**合并策略：**

1. 向前合并：检查前一页是否为空闲 Span
2. 向后合并：检查后一页是否为空闲 Span
3. 合并条件：相邻 Span 都空闲且合并后不超过 128 页
4. 合并后更新映射关系

### 使用示例

```cpp
// PageCache 通常不直接使用，而是通过 CentralCache 间接调用
// 以下为内部使用示例

size_t k = 5;
Span* span = PageCache::GetInstance()->NewSpan(k);

void* obj = ...;
Span* span = PageCache::GetInstance()->MapObjectToSpan(obj);
```

---

## 5. PageMap

### 模块简介

[PageMap](file:///d:/GitHub/Software-Projects-Collection/ConcurrentMemoryPool/PageMap.h) 提供页号到 Span 的高效映射，采用基数树（Radix Tree）数据结构，支持 O(1) 时间复杂度的查找。

### 三种实现

#### TCMalloc_PageMap1 - 单层基数树

适用于小规模地址空间（最多 2^BITS 页）。

**特点：**

- 简单直接，一次性分配所有空间
- 适用于 BITS 较小的场景

#### TCMalloc_PageMap2 - 二层基数树

适用于中等规模地址空间。

**特点：**

- 第一层存储指向第二层的指针
- 第二层存储实际的 Span 指针
- 按需分配第二层空间，节省内存

#### TCMalloc_PageMap3 - 三层基数树

适用于大规模地址空间（如 64 位系统）。

**特点：**

- 三层结构，支持更大的地址空间
- 完全按需分配，内存利用率高
- 适用于 BITS 较大的场景

### 主要方法

#### void* get(Number k) const

获取页号 k 对应的 Span 指针。

#### void set(Number k, void* v)

建立页号 k 与 Span 指针 v 的映射。

#### bool Ensure(Number start, size_t n)

确保映射 [start, start+n-1] 页号的空间已分配。

### 使用示例

```cpp
// 创建三层基数树
TCMalloc_PageMap3<64 - PAGE_SHIFT> pageMap;

// 建立映射
Span* span = ...;
PAGE_ID pageId = 100;
pageMap.set(pageId, span);

// 查找映射
Span* foundSpan = (Span*)pageMap.get(pageId);
```

---

## 6. ObjectPool

### 模块简介

[ObjectPool](file:///d:/GitHub/Software-Projects-Collection/ConcurrentMemoryPool/ObjectPool.h) 是一个通用对象池模板类，用于高效分配和释放固定类型的对象。通过内存池技术减少频繁 new/delete 操作的开销。

### 核心特性

- **内存复用**：释放的对象被放入自由链表，下次分配时直接复用
- **批量申请**：向系统批量申请内存，减少系统调用次数
- **定位 new**：支持调用对象的构造函数
- **显式析构**：释放时显式调用对象的析构函数

### 主要方法

#### T* New()

分配一个 T 类型的对象。

**工作流程：**

1. 检查自由链表是否有可用对象
2. 若有，从自由链表取出
3. 若没有，从大块内存中切分
4. 若大块内存不足，向系统申请新的大块内存（128KB）
5. 调用定位 new 执行构造函数
6. 返回对象指针

#### void Delete(T* obj)

释放一个 T 类型的对象。

**工作流程：**

1. 显式调用对象的析构函数
2. 将对象插入自由链表

### 使用示例

```cpp
#include "ObjectPool.h"

struct TreeNode {
    int _val;
    TreeNode* _left;
    TreeNode* _right;
    
    TreeNode() : _val(0), _left(nullptr), _right(nullptr) {}
};

void Example() {
    ObjectPool<TreeNode> pool;
    
    // 分配对象
    TreeNode* node1 = pool.New();
    TreeNode* node2 = pool.New();
    
    // 使用对象
    node1->_val = 10;
    node2->_val = 20;
    
    // 释放对象
    pool.Delete(node1);
    pool.Delete(node2);
}
```

### 性能优势

相比直接使用 new/delete，ObjectPool 具有以下优势：

1. **减少系统调用**：批量申请内存，减少与操作系统的交互
2. **减少内存碎片**：对象大小固定，内存布局紧凑
3. **提高缓存命中率**：连续内存分配，提高 CPU 缓存利用率
4. **降低锁竞争**：线程私有对象池，避免多线程竞争

---

## 7. ConcurrentAlloc

### 模块简介

[ConcurrentAlloc](file:///d:/GitHub/Software-Projects-Collection/ConcurrentMemoryPool/ConcurrentAlloc.h) 是内存池的对外接口，提供简洁的内存分配和释放函数。根据请求大小自动选择合适的分配策略。

### 核心特性

- **统一接口**：提供简单的分配和释放函数
- **自动路由**：根据大小自动选择 ThreadCache 或直接分配
- **TLS 支持**：自动为每个线程创建 ThreadCache
- **大内存支持**：支持超过 256KB 的大内存分配

### 主要函数

#### void* ConcurrentAlloc(size_t size)

分配 size 字节的内存。

**分配策略：**

1. 若 size > 256KB：
   - 计算需要的页数
   - 直接从 PageCache 获取对应页数的 Span
   - 返回 Span 的起始地址
2. 若 size <= 256KB：
   - 通过 TLS 获取当前线程的 ThreadCache
   - 从 ThreadCache 分配内存
   - 返回对象地址

#### void ConcurrentFree(void* ptr)

释放 ptr 指向的内存。

**释放策略：**

1. 通过 `MapObjectToSpan` 找到对应的 Span
2. 获取 Span 中对象的大小
3. 若大小 > 256KB：
   - 直接将 Span 归还给 PageCache
4. 若大小 <= 256KB：
   - 通过 TLS 获取当前线程的 ThreadCache
   - 从 ThreadCache 释放内存

### 使用示例

#### 基本使用

```cpp
#include "ConcurrentAlloc.h"

void BasicExample() {
    // 分配小内存
    void* ptr1 = ConcurrentAlloc(100);
    void* ptr2 = ConcurrentAlloc(256);
    void* ptr3 = ConcurrentAlloc(8192);
    
    // 使用内存
    // ...
    
    // 释放内存
    ConcurrentFree(ptr1);
    ConcurrentFree(ptr2);
    ConcurrentFree(ptr3);
}
```

#### 多线程使用

```cpp
#include "ConcurrentAlloc.h"
#include <thread>
#include <vector>

void ThreadWorker(int id) {
    std::vector<void*> ptrs;
    
    // 每个线程分配 1000 个对象
    for (int i = 0; i < 1000; i++) {
        ptrs.push_back(ConcurrentAlloc(64 + (i % 128)));
    }
    
    // 使用内存
    // ...
    
    // 释放所有对象
    for (void* ptr : ptrs) {
        ConcurrentFree(ptr);
    }
}

void MultiThreadExample() {
    const int threadCount = 4;
    std::vector<std::thread> threads;
    
    // 创建多个线程
    for (int i = 0; i < threadCount; i++) {
        threads.emplace_back(ThreadWorker, i);
    }
    
    // 等待所有线程完成
    for (auto& t : threads) {
        t.join();
    }
}
```

#### 大内存分配

```cpp
#include "ConcurrentAlloc.h"

void BigMemoryExample() {
    // 分配大内存（超过 256KB）
    void* ptr1 = ConcurrentAlloc(512 * 1024);  // 512KB
    void* ptr2 = ConcurrentAlloc(1024 * 1024); // 1MB
    
    // 使用内存
    // ...
    
    // 释放内存
    ConcurrentFree(ptr1);
    ConcurrentFree(ptr2);
}
```

### 性能对比

项目提供了性能测试程序 [Benchmark.cpp](file:///d:/GitHub/Software-Projects-Collection/ConcurrentMemoryPool/Benchmark.cpp)，可以对比 ConcurrentAlloc 和标准 malloc/free 的性能。

**测试参数：**

- `ntimes`：每次执行分配/释放的次数
- `nworks`：线程数
- `rounds`：执行轮数

**运行测试：**

```cpp
int main() {
    size_t n = 10000;
    cout << "==========================================================" << endl;
    BenchmarkConcurrentMalloc(n, 4, 10);  // 测试 ConcurrentAlloc
    cout << endl << endl;
    BenchmarkMalloc(n, 4, 10);            // 测试 malloc/free
    cout << "==========================================================" << endl;
    return 0;
}
```

---

## 内存分配流程图

```
ConcurrentAlloc(size)
    │
    ├─ size > 256KB
    │   └─ PageCache::NewSpan(kPage)
    │       └─ SystemAlloc(kPage)
    │
    └─ size <= 256KB
        └─ ThreadCache::Allocate(size)
            │
            ├─ 自由链表有对象
            │   └─ FreeList::Pop()
            │
            └─ 自由链表为空
                └─ ThreadCache::FetchFromCentralCache()
                    └─ CentralCache::FetchRangeObj()
                        │
                        ├─ 有可用 Span
                        │   └─ 从 Span 取出对象
                        │
                        └─ 无可用 Span
                            └─ CentralCache::GetOneSpan()
                                └─ PageCache::NewSpan(k)
                                    │
                                    ├─ k 页桶有空闲 Span
                                    │   └─ 直接返回
                                    │
                                    ├─ 更大页桶有空闲 Span
                                    │   └─ 分割并返回
                                    │
                                    └─ 无空闲 Span
                                        └─ SystemAlloc(128页)
```

## 内存释放流程图

```
ConcurrentFree(ptr)
    │
    ├─ size > 256KB
    │   └─ PageCache::ReleaseSpanToPageCache()
    │       └─ SystemFree(ptr)
    │
    └─ size <= 256KB
        └─ ThreadCache::Deallocate(ptr, size)
            │
            ├─ 自由链表未满
            │   └─ FreeList::Push(ptr)
            │
            └─ 自由链表已满
                └─ ThreadCache::ListTooLong()
                    └─ CentralCache::ReleaseListToSpans()
                        │
                        ├─ Span 仍在使用
                        │   └─ 插入 Span 的自由链表
                        │
                        └─ Span 已完全释放
                            └─ PageCache::ReleaseSpanToPageCache()
                                └─ 尝试合并相邻 Span
```

## 注意事项

1. **内存大小限制**：ThreadCache 最多分配 256KB，超过此大小直接从 PageCache 分配
2. **线程安全**：ConcurrentAlloc 和 ConcurrentFree 是线程安全的，可以在多线程环境中使用
3. **内存对齐**：所有分配的内存都会按特定规则对齐，可能比请求的大小略大
4. **对象池复用**：释放的内存会被放入自由链表，下次分配时优先复用
5. **Span 合并**：PageCache 会尝试合并相邻的空闲 Span，减少内存碎片

## 编译和运行

### 编译

使用 Visual Studio 打开 `ConcurrentMemoryPool.vcxproj` 项目文件进行编译。

### 运行测试

1. **单元测试**：运行 `UnitTest.cpp` 中的测试用例
2. **性能测试**：运行 `Benchmark.cpp` 中的性能对比测试

### 测试用例

项目提供了多个测试用例（在 [UnitTest.cpp](file:///d:/GitHub/Software-Projects-Collection/ConcurrentMemoryPool/UnitTest.cpp) 中）：

- `TLSTest()` - 测试线程本地存储
- `TestConcurrentAlloc1()` - 基本分配测试
- `TestConcurrentAlloc2()` - 批量分配测试
- `MultiThreadAllocTest()` - 多线程分配测试
- `BigAlloc()` - 大内存分配测试

## 总结

ConcurrentMemoryPool 是一个高效的高并发内存池实现，通过三级缓存架构（ThreadCache、CentralCache、PageCache）实现了：

1. **高性能**：线程私有缓存减少锁竞争，批量操作减少系统调用
2. **低碎片**：固定大小分配，Span 合并减少内存碎片
3. **可扩展**：支持从 1 字节到超大内存的分配
4. **线程安全**：完善的锁机制保证多线程环境下的正确性

该实现适用于需要频繁分配和释放小对象的多线程应用程序，如网络服务器、游戏引擎等场景。
