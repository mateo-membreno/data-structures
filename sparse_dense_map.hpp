#pragma once

#include <cstddef>
#include <functional>
#include <memory>
#include <utility>

template <
    typename Key,
    typename Value,
    typename Hash      = std::hash<Key>,
    typename KeyEqual  = std::equal_to<Key>,
    typename Allocator = std::allocator<std::pair<const Key, Value>>
>
class SparseDenseMap
{
public:
    explicit SparseDenseMap(size_t initial_capacity = 16)
        : sparse_(initial_capacity * 2, EMPTY)
        , dense_()
        , size_(0)
    {
        dense_.reserve(initial_capacity);
    }

    ~SparseDenseMap() = default;

    void insert(const Key& key, const Value& value){
        size_t index = Hash{}(key) % sparse_.size();
        while(sparse_[index] != EMPTY && sparse_[index] != TOMB){
            if(KeyEqual{}(key, dense_[sparse_[index]].first)){
                dense_[sparse_[index]].second = value;
                return;
            }
             index = (index + 1) % sparse_.size();
        }
        sparse_[index] = size_;
        dense_.emplace_back(key, value);
        size_++;
        if(size_ * 2 > sparse_.size()){
            resize();
        }
    }

    // think of append method if workds for generik value type

    void remove(Key key){
        return;
    }

    bool contains(const Key& key){
        size_t index = Hash{}(key) % sparse_.size();
        while(sparse_[index] != EMPTY){
            if(sparse_[index] != TOMB && KeyEqual{}(key, dense_[sparse_[index]].first)){
                return true;
            }
            index = (index + 1) % sparse_.size();
        }
        return false;
    }

    Value get(const Key& key){
        size_t index = Hash{}(key) % sparse_.size();
        while(sparse_[index] != EMPTY){
             if (sparse_[index] != TOMB && KeyEqual{}(key, dense_[sparse_[index]].first)){
                return dense_[sparse_[index]].second;
             }
             index = (index + 1) % sparse_.size();
        }
        throw std::out_of_range("key not found");
    }

private:
    static constexpr size_t EMPTY = SIZE_MAX;
    static constexpr size_t TOMB = SIZE_MAX - 1;

    std::vector<size_t> sparse_;
    std::vector<std::pair<const Key, Value>> dense_;
    size_t size_;

    void resize(){
        sparse_.assign(sparse_.size() * 2, EMPTY);
        size_t index;
        for (size_t i = 0; i < size_; i++){
            index = Hash{}(dense_[i].first) % sparse_.size();
            while(sparse_[index] != EMPTY){
                index = (index + 1) % sparse_.size();
            }
            sparse_[index] = i;

        }
    }
};
