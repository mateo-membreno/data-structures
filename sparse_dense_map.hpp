#pragma once

#include <cstddef>
#include <functional>
#include <memory>
#include <utility>
#include <vector>
#include <stdexcept>

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
        auto [slot, found] = find_slot(key);
        if (found){
            dense_[sparse_[slot]].second = value;
        } else {
            sparse_[slot] = size_;
            dense_.emplace_back(key, value);
            size_++;
            if(size_ * 2 > sparse_.size()){
                resize();
            }
        }
    }

    // If key exists, combines existing value with new value using combine(existing, value).
    // If key doesn't exist, inserts value as-is. Defaults to addition.
    template <typename Combine = std::plus<Value>>
    Value append(const Key& key, const Value& value, Combine combine = {}){
        auto [slot, found] = find_slot(key);
        if (found){
            Value& existing = dense_[sparse_[slot]].second;
            existing = combine(existing, value);
            return existing;
        } else {
            sparse_[slot] = size_;
            dense_.emplace_back(key, value);
            size_++;
            if(size_ * 2 > sparse_.size()){
                resize();
            }
            return value;
        }
    }

    void remove(const Key& key){
        auto [slot, found] = find_slot(key);
        if (!found) return;

        size_t dense_idx = sparse_[slot];
        size_t last_idx = dense_.size() - 1;
        if (dense_idx != last_idx) {
            auto [last_slot, _] = find_slot(dense_[last_idx].first);
            dense_[dense_idx] = std::move(dense_[last_idx]);
            sparse_[last_slot] = dense_idx;
        }
        dense_.pop_back();
        sparse_[slot] = TOMB;
        size_--;
    }

    Value get(const Key& key){
        auto [slot, found] = find_slot(key);
        if (!found) throw std::out_of_range("key not found");
        return dense_[sparse_[slot]].second;
    }

    // Returns true if key exists
    bool contains(const Key& key){
        return find_slot(key).found;
    }

private:
    static constexpr size_t EMPTY = SIZE_MAX;
    static constexpr size_t TOMB  = SIZE_MAX - 1;

    struct SlotResult { 
        size_t slot; 
        bool found; 
    };

    std::vector<size_t> sparse_;
    std::vector<std::pair<Key, Value>> dense_;
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

    // Single probe: returns the existing slot if found, else the best insertion
    // slot is the first tombstone een or the first empty;
    SlotResult find_slot(const Key& key){
        size_t index = Hash{}(key) % sparse_.size();
        size_t first_tomb = EMPTY;
        while (sparse_[index] != EMPTY) {
            if (sparse_[index] == TOMB) {
                if (first_tomb == EMPTY){
                    first_tomb = index;
                }
            } else if (KeyEqual{}(key, dense_[sparse_[index]].first)) {
                return {index, true};
            }
            index = (index + 1) % sparse_.size();
        }
        return {first_tomb != EMPTY ? first_tomb : index, false};
    }
};
