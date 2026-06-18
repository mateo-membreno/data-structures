#pragma once
#include <cstddef>
#include "hash_utils.h"

template <typename T>
class UnorderedSet {
public:
    UnorderedSet() : data_(new T[8]), states_(new State[8]), size_(0), capacity_(8) {
        for (size_t i = 0; i < capacity_; i++){
            states_[i] = State::Empty;
        }
    }

    bool contains(const T& val){
        uint64_t index = fnv1a(val) % capacity_;
        while(states_[index] != State::Empty){
            if (states_[index] == State::Occupied && data_[index] == val){
                return true;
            }
            index = (index + 1) % capacity_;
        }
        return false;

    }

    void insert(const T& val){
        if (size_ * 2 > capacity_){
            resize();
        }

        uint64_t index = fnv1a(val) % capacity_;

        while(states_[index] == State::Occupied){
            if (data_[index] == val) return;
            index = (index + 1) % capacity_;
        }

        states_[index] = State::Occupied;
        data_[index] = val;
        size_++;



    }

    size_t size() {
        return size_;
    }

    void remove(const T& val){
        uint64_t index = fnv1a(val) % capacity_;

        while(states_[index] != State::Empty){
            if (states_[index] == State::Occupied && data_[index] == val){
                states_[index] = State::Tombstone;
                size_--;
                return;
            }
            index = (index + 1) % capacity_;
        }
        return;
    }



private:
    enum class State : uint8_t { Empty, Occupied, Tombstone };
    T* data_;
    State* states_;
    size_t size_;
    size_t capacity_;

    void resize(){
        const size_t new_capacity = capacity_ * 2;
        T* bigger = new T[new_capacity];
        State* new_states = new State[new_capacity];
        for (size_t i = 0; i < new_capacity; i++){
            new_states[i] = State::Empty;
        }
        

        for (size_t i = 0; i < capacity_; i++){
            uint64_t index;
            if(states_[i] == State::Occupied){
                index = fnv1a(data_[i]) % new_capacity;

                while(new_states[index] == State::Occupied){
                    index = (index + 1) % new_capacity;
                }

                new_states[index] = State::Occupied;
                bigger[index] = data_[i];

            }
        }
        delete[] data_;
        delete[] states_;
        data_ = bigger;
        states_ = new_states;
        capacity_ = new_capacity;

    }
};