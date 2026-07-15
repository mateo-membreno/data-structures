#pragma once
#include <cstddef>
#include <cmath>
#include <numeric>
#include <string>
#include "hash_utils.h"

template <typename T>
class BloomFilter {
public:
   BloomFilter(float false_positive_rate, size_t expected_items, size_t checkpoint_rate, string checkpoint_file) {
    num_bits_ = (expected_items * std::log(false_positive_rate)) / (std::pow(std::log(2), 2));

    num_hashes_ = num_bits_ / expected_items;
    data_ = new uint8_t[(num_bits_ + 7) / 8]();
    count_ = 0;
    checkpoint_rate_ = checkpoint_rate;
    checkpoint_file_ = checkpoint_file;
   }

   bool insert(T data){
        uint64_t hash_a = fnv1a(data);
        uint64_t hash_b = fnv1a(hash_a);

        if (hash_b == 0) hash_b = 1;
        while (std::gcd(hash_b, static_cast<uint64_t>(num_bits_)) != 1) hash_b++;

        bool inserted = false;
        for (size_t i = 0; i < num_hashes_; i++) {
            size_t index = (hash_a + i * hash_b) % num_bits_;
            if (!get_bit(index)) inserted = true;
            set_bit(index);
        }

        if (inserted) count_++;
        return inserted;
   }


private:
    uint8_t *data_;
    size_t num_bits_; // how many bits wide the filter should be(i.e. max items it can hold) 
    size_t num_hashes_; // how many bits to check in filter
    size_t count_; // number of present elements
    size_t checkpoint_rate_; // after how many inserts to chechpoint
    string checkpoint_file_; 

    void set_bit(size_t index) {
        if (index >= num_bits_) return;
        
        size_t byte_idx = index / 8;
        size_t bit_idx  = index % 8;

        data_[byte_idx] |= (1 << bit_idx);
    }

    bool get_bit(size_t index) const {
        if (index >= num_bits_) return false;

        size_t byte_idx = index / 8;
        size_t bit_idx  = index % 8;

        return data_[byte_idx] & (1 << bit_idx);
    }
};
