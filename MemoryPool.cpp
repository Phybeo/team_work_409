#include <atomic>
#include "MemoryPool.h"

namespace memoryPool
{
MemoryPool::MemoryPool(size_t BlockSize)
    : BlockSize_(BlockSize),
      freeList_(nullptr) // 初始化原子 指针
{}

MemoryPool::~MemoryPool()
{
    // 把连续的block删除
    Slot* cur = firstBlock_;
    while (cur)
    {
        Slot* next = cur->next;
        operator delete(reinterpret_cast<void*>(cur));
        cur = next;
    }
}

void MemoryPool::init(size_t size)
{
    assert(size > 0);
    SlotSize_ = size;
    firstBlock_ = nullptr;
    curSlot_ = nullptr;
    freeList_.store(nullptr, std::memory_order_relaxed); // 使用原子存储初始化空闲链表
    lastSlot_ = nullptr;
}

void* MemoryPool::allocate()
{
    // 优先使用空闲链表中的内存槽，使用无锁CAS操作
    Slot* oldHead = freeList_.load(std::memory_order_relaxed);
    do {
        if (oldHead == nullptr) break; // 空闲链表为空，跳出循环
        // 尝试更新freeList_的头部指针
    } while (!freeList_.compare_exchange_weak(oldHead, oldHead->next, 
                                             std::memory_order_relaxed,
                                             std::memory_order_relaxed));
    
    if (oldHead != nullptr) {
        return oldHead; // 成功从空闲链表获取内存
    }

    // 空闲链表为空，需要从内存块中分配
    Slot* temp;
    {
        std::lock_guard<std::mutex> lock(mutexForBlock_);
        if (curSlot_ >= lastSlot_) {
            // 当前内存块已无内存槽可用，开辟一块新的内存
            allocateNewBlock();
        }

        temp = curSlot_;
        // 这里不能直接 curSlot_ += SlotSize_ 因为curSlot_是Slot*类型
        curSlot_ += SlotSize_ / sizeof(Slot);
    }

    return temp;
}

void MemoryPool::deallocate(void* ptr)
{
    if (ptr) {
        // 回收内存，将内存通过头插法插入到空闲链表中，使用无锁CAS操作
        Slot* newHead = reinterpret_cast<Slot*>(ptr);
        Slot* oldHead = freeList_.load(std::memory_order_relaxed);
        do {
            newHead->next = oldHead;
            // 尝试将新节点插入到空闲链表头部
        } while (!freeList_.compare_exchange_weak(oldHead, newHead, 
                                                 std::memory_order_relaxed,
                                                 std::memory_order_relaxed));
    }
}

void MemoryPool::allocateNewBlock()
{
    //std::cout << "申请一块内存块, SlotSize: " << SlotSize_ << std::endl;
    // 头插法插入新的内存块
    void* newBlock = operator new(BlockSize_);
    reinterpret_cast<Slot*>(newBlock)->next = firstBlock_;
    firstBlock_ = reinterpret_cast<Slot*>(newBlock);

    char* body = reinterpret_cast<char*>(newBlock) + sizeof(Slot*);
    size_t paddingSize = padPointer(body, SlotSize_); // 计算对齐需要填充内存的大小
    curSlot_ = reinterpret_cast<Slot*>(body + paddingSize);

    // 超过该标记位置，则说明该内存块已无内存槽可用，需向系统申请新的内存块
    lastSlot_ = reinterpret_cast<Slot*>(reinterpret_cast<size_t>(newBlock) + BlockSize_ - SlotSize_ + 1);

    freeList_.store(nullptr, std::memory_order_relaxed); // 使用原子操作设置空闲链表为空
}

    // 让指针对齐到槽大小的倍数位置
size_t MemoryPool::padPointer(char* p, size_t align)
{
        // align 是槽大小
    return (align - reinterpret_cast<size_t>(p)) % align;
}

void HashBucket::initMemoryPool()
{
    for (int i = 0; i < MEMORY_POOL_NUM; i++)
    {
        getMemoryPool(i).init((i + 1) * SLOT_BASE_SIZE);
    }
}

    // 单例模式
MemoryPool& HashBucket::getMemoryPool(int index)
{
    static MemoryPool memoryPool[MEMORY_POOL_NUM];
    return memoryPool[index];
}
} // namespace memoryPool