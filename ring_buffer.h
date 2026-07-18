#pragma once
#include <algorithm>
#include <cstddef>
#include <cstring>
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


class VarLen {
public:
    // also single threaded, but now allocating variable length data
    VarLen(uint64_t total_bytes) : data_(new uint8_t[total_bytes]), byte_capacity_(total_bytes), size_(0), head_(0), tail_(0) {}

    ~VarLen() {
        delete[] data_;
    }

    bool read(uint8_t *dest, uint64_t len) {
        if (len > size_) return false;

        uint64_t first_chunk = std::min(len, byte_capacity_ - head_);
        std::memcpy(dest, data_ + head_, first_chunk);
        if (first_chunk < len) {
            std::memcpy(dest + first_chunk, data_, len - first_chunk);
        }

        head_ = (head_ + len) % byte_capacity_;
        size_ -= len;
        return true;
    }

    bool write(const uint8_t *value, uint64_t len){
        if (len > byte_capacity_ - size_) return false;

        uint64_t first_chunk = std::min(len, byte_capacity_ - tail_);
        std::memcpy(data_ + tail_, value, first_chunk);
        if (first_chunk < len) {
            std::memcpy(data_, value + first_chunk, len - first_chunk);
        }

        tail_ = (tail_ + len) % byte_capacity_;
        size_ += len;
        return true;
    }

    bool is_empty(){
        return size_ == 0;
    }
    bool is_full(){
        return size_ == byte_capacity_;
    }

    private:

    uint8_t *data_;
    uint64_t byte_capacity_;
    uint64_t size_;
    uint64_t head_;
    uint64_t tail_;

};

}  // namespace ring_buffer
