#include <iostream>
#include <cassert>
#include <thread>
#include <vector>
#include "MemoryPool.h" // 确保包含正确的头文件路径
 
namespace memoryPool {
// 测试用例1：基础内存分配/释放
void testBasicMemory() {
    HashBucket::initMemoryPool(); // 初始化内存池
    
    // 测试小内存分配（<512字节）
    void* smallMem = HashBucket::useMemory(32);
    assert(smallMem != nullptr);
    std::cout << "分配32字节内存成功: " << smallMem << std::endl;
    
    // 释放内存
    HashBucket::freeMemory(smallMem, 32);
    std::cout << "释放32字节内存成功" << std::endl;
    
    // 测试大内存分配（>512字节）
    void* largeMem = HashBucket::useMemory(1024);
    assert(largeMem != nullptr);
    std::cout << "分配1024字节内存成功: " << largeMem << std::endl;
    
    HashBucket::freeMemory(largeMem, 1024);
}}


