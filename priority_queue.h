#pragma once
#include <cstddef>
#include <functional>
#include <stdexcept>
#include <utility>


template <typename T, typename Compare = std::less<T>>
class PriorityQueue {
public:
    PriorityQueue() : data_(new T[8]), size_(0), capacity_(8), comp_() {}
    explicit PriorityQueue(Compare comp) : data_(new T[8]), size_(0), capacity_(8), comp_(comp) {}

    ~PriorityQueue() { 
        delete[] data_; 
    }

    void push(const T& val) {
        if (size_ == capacity_){
            grow();
        }
        data_[size_] = val;
        size_++;
        sift_up(size_ - 1);
    }

    const T& top() const {
        if (size_ == 0) throw std::out_of_range("top() on empty PriorityQueue");
        return data_[0];
    }

    void pop() {
        if (size_ > 0){
            data_[0] = data_[--size_];
            sift_down(0);
        } 
    }

    size_t size() const { 
        return size_; 
    }

    bool empty() const {
        return size_ == 0; 
    }

private:
    T* data_;
    size_t size_;
    size_t capacity_;
    Compare comp_;

    void grow() {
        capacity_ *= 2;
        T* bigger = new T[capacity_];
        for (size_t i = 0; i < size_; ++i)
            bigger[i] = data_[i];
        delete[] data_;
        data_ = bigger;
    }

    void sift_up(size_t i) {
        while (i > 0) {
            size_t parent = (i - 1) / 2;
            if (comp_(data_[parent], data_[i])) {
                std::swap(data_[parent], data_[i]);
                i = parent;
            } else {
                break;
            }
        }
    }

    void sift_down(size_t i) {
        while (true) {
            size_t top = i;
            size_t l = 2 * i + 1;
            size_t r = 2 * i + 2;
            if (l < size_ && comp_(data_[top], data_[l])) top = l;
            if (r < size_ && comp_(data_[top], data_[r])) top = r;
            if (top == i) break;
            std::swap(data_[i], data_[top]);
            i = top;
        }
    }
};