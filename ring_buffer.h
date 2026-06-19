#pragma once
#include <atomic>
#include <cstddef>

template <typename T>
class RingBuffer {
public:
    // capacity is number of T objects, T must be default constructable and destructable
    // TODO: fix lots of cache invalidation for atomic head and tail accesses 
    RingBuffer(uint64_t capacity) : data_(new T[capacity]), capacity_(capacity), head_(0), tail_(0) {}

    ~RingBuffer() {
        delete[] data_;
    }

    T read(){
        if (is_empty()) return T{};
        uint64_t head = head_.load(std::memory_order_relaxed);
        T val = data_[head];
        head_.store((head + 1) % capacity_, std::memory_order_release);
        return val;
    }
    void write(const T &value){
        if (!is_full()){
            uint64_t tail = tail_.load(std::memory_order_relaxed);
            data_[tail] = value;
            tail_.store((tail + 1) % capacity_, std::memory_order_release);
        }
    }

    bool is_empty(){
        return head_.load(std::memory_order_acquire) == tail_.load(std::memory_order_acquire);
    }
    bool is_full(){
        return (tail_.load(std::memory_order_acquire) + 1) % capacity_ == head_.load(std::memory_order_acquire);
    }

private:
    T* data_;
    uint64_t capacity_;
    std::atomic<uint64_t> head_;
    std::atomic<uint64_t> tail_;
};
