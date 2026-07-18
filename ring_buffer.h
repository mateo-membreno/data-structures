#pragma once
#include <cstddef>
#include <stdexcept>

namespace ring_buffer {

template <typename T>
class Basic {
public:
    // capacity is number of T objects, T must be default constructable and destructable
    Basic(uint64_t capacity) : data_(new T[capacity]), capacity_(capacity), size_(0), head_(0), tail_(0) {}

    ~Basic() {
        delete[] data_;
    }

    T read(){
        if (is_empty()) throw std::out_of_range("read from empty RingBuffer");

        size_--;
        T value = data_[head_];
        head_ = (head_ + 1) % capacity_;
        return value;
    }
    bool write(const T &value){
        if (is_full()) return false;

        data_[tail_] = value;
        tail_ = (tail_ + 1) % capacity_;
        size_++;
        return true;
    }

    bool is_empty(){
        return size_ == 0;
    }
    bool is_full(){
      return size_ == capacity_;
    }

private:
    T* data_;
    uint64_t capacity_;
    uint64_t size_;
    uint64_t head_;
    uint64_t tail_;
};

}  // namespace ring_buffer
