#pragma once
#include <atomic>
#include <cstddef>

template <typename T>
class AtomicVector {
public:
    AtomicVector() : data_(new T[8]), size_(0), capacity_(8) {
    }
    AtomicVector(size_t n, const T &val) : data_(new T[n]), size_(n), capacity_(n) {
        for (size_t i = 0; i < n; i++){
            data_[i] = val;
        }
    }
    ~AtomicVector();

    void push_back(const T& val){
        size_t sz = size_.load(std::memory_order_relaxed);
        if (sz == capacity_.load(std::memory_order_relaxed)){
            grow(sz + 1);
        }
        data_.load(std::memory_order_relaxed)[sz] = val;
        size_.fetch_add(std::memory_order_release);
    }
    void pop_back(){
        if (size_.load(std::memory_order_acquire) > 0){
            size_.fetch_sub(1, std::memory_order_relaxed); 
        }
    }
    void remove(size_t index);

    T& operator[](size_t index){
        return data_.load(std::memory_order_acquire)[index];
    }

    size_t size() const{
        return size_.load(std::memory_order_acquire);
    }

    size_t capacity() const{
        return capacity_.load(std::memory_order_acquire);
    }

    void resize(size_t n);
    void reserve(size_t n);

    bool empty() const{
        return size_.load(std::memory_order_acquire) == 0;
    }

private:
    std::atomic<T*>     data_;
    std::atomic<size_t> size_;
    std::atomic<size_t> capacity_;

    void grow(size_t min_cap); 
};
