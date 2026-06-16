#pragma once
#include <cstddef>
#include <shared_mutex>

template <typename T>
class MVector {
public:
    MVector() : data_(new T[8]), size_(0), capacity_(8) {
    }
    MVector(size_t n, const T &val) : data_(new T[n]), size_(n), capacity_(n) {
        for (size_t i = 0; i < n; i++){
            data_[i] = val;
        }
    }
    ~MVector() { 
        delete[] data_; 
    }

    void push_back(const T &val){
        std::unique_lock<std::shared_mutex> lock(mutex_); 
        if (size_ == capacity_){
            resize();
        }
        data_[size_] = val;
        size_++;
    }

    void pop_back(){
        std::unique_lock<std::shared_mutex> lock(mutex_);
        if (size_ > 0) size_--;
    }

    void remove(size_t index){
        std::unique_lock<std::shared_mutex> lock(mutex_);
        if (index >= size_) return;
        for (size_t i = index + 1; i < size_; i++){
            data_[i-1] = data_[i];
        }
        size_--;
    }
    
    T &operator[](size_t index){
        std::unique_lock<std::shared_mutex> lock(mutex_); 
        return data_[index];
    }

    size_t size() const{
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return size_;
    }
    
     size_t capacity() const{
        std::shared_lock<std::shared_mutex> lock(mutex_); 
        return capacity_;
    }

    void resize(size_t n){
        std::unique_lock<std::shared_mutex> lock(mutex_);
        if (n > capacity_){
            T *bigger = new T[n];
            for (size_t i = 0; i < size_; i++){
                bigger[i] = data_[i];
            }
            delete[] data_;
            data_ = bigger;
            capacity_ = n;
        } 
        if (n > size_){
            for (size_t i = size_; i < capacity_; i++){
                data_[i] = T{};
            }
        }

        size_ = n;
    }

    void reserve(size_t n){
        std::unique_lock<std::shared_mutex> lock(mutex_);
        T *reserved = new T[n];
        for (size_t i = 0; i < size_; i++){
            reserved[i] = data_[i];
        }
        delete[] data_;
        data_ = reserved;
        capacity_ = n;
    }

    bool empty() const{
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return size_ == 0;
    }

private:
    T *data_;
    size_t size_;
    size_t capacity_;
    mutable std::shared_mutex mutex_;

    void resize(){
        T *bigger = new T[capacity_ * 2];
        for (size_t i = 0; i < size_; i++){
            bigger[i] = data_[i];
        }
        delete[] data_;
        data_ = bigger;
        capacity_ *= 2;
    }
};
