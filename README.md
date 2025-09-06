# 高性能内存池项目

这是一个基于无锁数据结构的高性能内存池实现，专为多线程环境优化。

## 项目特点

- 基于无锁算法实现的内存池，提高多线程环境下的性能
- 使用CAS(Compare-And-Swap)操作管理空闲槽链表，避免锁竞争
- 支持不同大小的内存分配，通过HashBucket管理多个内存池
- 提供与标准new/delete兼容的接口

## 代码结构

- `MemoryPool.h`: 内存池类的定义和接口声明
- `MemoryPool.cpp`: 内存池的具体实现，包括无锁算法
- `PerformanceTest.cpp`: 性能测试程序，用于比较自定义内存池与标准new/delete的性能差异
- `compile.bat`: Windows环境下的编译脚本

## 编译指南

在Windows环境下，可以直接运行编译脚本：

```cmd
compile.bat
```

该脚本会尝试使用GCC或MSVC编译器来编译代码。如果没有找到编译器，脚本会提示用户安装编译器或使用Visual Studio Developer Command Prompt。

## 运行性能测试

编译成功后，会生成`PerformanceTest.exe`可执行文件。运行该程序即可进行性能测试：

```cmd
PerformanceTest.exe
```

性能测试会执行以下操作：

1. 初始化内存池
2. 进行预热操作，减少系统缓存对测试结果的影响
3. 运行多线程测试，比较自定义内存池与标准new/delete的性能
4. 输出测试结果，显示两种方法的执行时间

## 测试参数说明

性能测试使用以下参数：

- 线程数量: 8
- 每线程操作次数: 100,000
- 最大对象大小: 256字节
- 分配/释放比例: 70%分配，30%释放

## 性能测试结果解读

测试结果会显示自定义内存池和标准new/delete的执行时间。在多线程环境下，由于使用了无锁数据结构，自定义内存池通常会比标准new/delete有更好的性能表现，特别是在高并发场景下。

## 如何在项目中使用

1. 包含头文件：`#include "MemoryPool.h"`
2. 初始化内存池：`memoryPool::HashBucket::initMemoryPool()`
3. 使用内存池分配内存：`void* ptr = memoryPool::HashBucket::useMemory(size)`
4. 释放内存：`memoryPool::HashBucket::freeMemory(ptr, size)`
5. 或者使用更方便的模板函数：
   - 分配并构造对象：`T* obj = memoryPool::newElement<T>(args...)`
   - 析构并释放对象：`memoryPool::deleteElement(obj)`

## 注意事项

- 内存池适用于频繁分配/释放小块内存的场景
- 对于大于512字节的内存分配，会自动回退到使用标准new/delete
- 内存池使用单例模式，全局只有一套内存池实例
- 在程序结束前，确保释放所有分配的内存以避免内存泄漏
