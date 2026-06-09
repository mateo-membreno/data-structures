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


private:
    static constexpr size_t EMPTY = SIZE_MAX;

    std::vector<size_t> sparse_;
    std::vector<std::pair<const Key, Value>> dense_;
    size_t size_;

};
