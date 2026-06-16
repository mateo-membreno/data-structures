#pragma once
#include <cstddef>

template <typename T>
class Queue {
public:
    Queue() : data_(new T[8]), front_(0), back_(0), size_(0), capacity_(8) {
    }
    Queue(size_t n, const T &val) : data_(new T[n]), front_(0), back_(0), size_(n), capacity_(n) {
        for (size_t i = 0; i < n; i++){
            data_[i] = val;
        }
    }
    ~Queue() { 
        delete[] data_; 
    }

    void enqueue(const T &val){
        if (capacity_ == size_){
            resize();
        }
        data_[back_] = val;
        back_ = (back_ + 1) % capacity_;
        size_++;
    }

    T dequeue(){
        if (size_ < 1) return T{};
        T val = front();
        front_ = (front_ + 1) % capacity_;
        size_--;
        return val;
    }

    T front(){
        return data_[front_];
    }

    size_t size() const{
        return size_;
    }

private:
    T *data_;
    size_t front_;
    size_t back_;
    size_t size_;
    size_t capacity_;

    void resize(){
        T *bigger = new T[capacity_ * 2];
        for (size_t i = 0; i < size_; i++){
            bigger[i] = data_[(front_ + i) % capacity_];
        }
        delete[] data_;
        data_ = bigger;
        front_ = 0;
        back_ = size_;
        capacity_ *= 2;
    }


};
