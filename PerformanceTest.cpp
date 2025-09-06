#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <random>
#include <iomanip>
#include "MemoryPool.h"

// 定义测试参数
const int NUM_THREADS = 8;          // 线程的数量
const int OPS_PER_THREAD = 100000;  // 每个线程执行的操作次数
const int MAX_OBJECT_SIZE = 256;    // 最大对象大小

// 用于随机生成对象大小和操作类型的随机数生成器
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> size_dist(1, MAX_OBJECT_SIZE);
std::uniform_int_distribution<> op_dist(0, 99); // 用于随机决定操作类型

// 使用自定义内存池进行测试
void testMemoryPool(int threadId) {
    std::vector<void*> allocatedObjects;
    allocatedObjects.reserve(OPS_PER_THREAD);
    
    for (int i = 0; i < OPS_PER_THREAD; ++i) {
        int size = size_dist(gen);
        int op = op_dist(gen);
        
        if (op < 70 || allocatedObjects.empty()) {  // 70%的概率进行分配
            void* ptr = memoryPool::HashBucket::useMemory(size);
            allocatedObjects.push_back(ptr);
        } else {  // 30%的概率进行释放
            int index = rand() % allocatedObjects.size();
            memoryPool::HashBucket::freeMemory(allocatedObjects[index], size_dist(gen));
            allocatedObjects[index] = allocatedObjects.back();
            allocatedObjects.pop_back();
        }
    }
    
    // 释放剩余的对象
    for (auto ptr : allocatedObjects) {
        memoryPool::HashBucket::freeMemory(ptr, size_dist(gen));
    }
}

// 使用标准new/delete进行测试
void testNewDelete(int threadId) {
    std::vector<void*> allocatedObjects;
    allocatedObjects.reserve(OPS_PER_THREAD);
    
    for (int i = 0; i < OPS_PER_THREAD; ++i) {
        int size = size_dist(gen);
        int op = op_dist(gen);
        
        if (op < 70 || allocatedObjects.empty()) {  // 70%的概率进行分配
            void* ptr = operator new(size);
            allocatedObjects.push_back(ptr);
        } else {  // 30%的概率进行释放
            int index = rand() % allocatedObjects.size();
            operator delete(allocatedObjects[index]);
            allocatedObjects[index] = allocatedObjects.back();
            allocatedObjects.pop_back();
        }
    }
    
    // 释放剩余的对象
    for (auto ptr : allocatedObjects) {
        operator delete(ptr);
    }
}

// 运行多线程测试
void runMultithreadedTest(void (*testFunc)(int), const std::string& testName) {
    std::vector<std::thread> threads;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // 创建并启动线程
    for (int i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back(testFunc, i);
    }
    
    // 等待所有线程完成
    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << testName << " 执行时间: " << duration.count() << " 毫秒" << std::endl;
}

int main() {
    // 初始化内存池
    memoryPool::HashBucket::initMemoryPool();
    
    std::cout << "====== 内存池性能测试 ======" << std::endl;
    std::cout << "线程数: " << NUM_THREADS << std::endl;
    std::cout << "每线程操作次数: " << OPS_PER_THREAD << std::endl;
    std::cout << "最大对象大小: " << MAX_OBJECT_SIZE << " 字节" << std::endl;
    std::cout << "===========================" << std::endl;
    
    // 先进行一些预热操作，减少系统缓存对测试结果的影响
    std::cout << "预热中..." << std::endl;
    for (int i = 0; i < 5; ++i) {
        testMemoryPool(0);
        testNewDelete(0);
    }
    
    std::cout << "开始测试..." << std::endl;
    
    // 测试内存池性能
    runMultithreadedTest(testMemoryPool, "自定义内存池");
    
    // 测试标准new/delete性能
    runMultithreadedTest(testNewDelete, "标准new/delete");
    
    std::cout << "测试完成!" << std::endl;
    
    return 0;
}