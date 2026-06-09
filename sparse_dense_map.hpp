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

    insert(Key key, Value value){

    }

    delete(Key key){

    }

    bool contains(Key key){
        if(Hash{}(key) == EMPTY){
            return false;
        }
        return true;
    }

    Value get(const Key& key){
        Hash hs();
        size_t index = hs(key) % sparse_.size();
        while(sparse_[index] != EMPTY){
             if (KeyEqual(key, dense[sparse_[index]].first)){
                return dense[sparse_[index].second];
             }
             index++;
        }
        throw std::out_of_range("key not found");
    }
private:
    static constexpr size_t EMPTY = SIZE_MAX;
    static constexpr size_t TOMB = SIZE_MAX - 1;

    std::vector<size_t> sparse_;
    std::vector<std::pair<const Key, Value>> dense_;
    size_t size_;

};
