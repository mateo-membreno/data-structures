#pragma once
#include <cstddef>
#include <cmath>
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
        uint64_t hash_b = fnv1a_64(&hash_a, sizeof(hash_a));

        bool already_present = true;
        for (size_t i = 0; i < num_hashes_; i++) {
            size_t index = (hash_a + i * hash_b) % num_bits_;
            if (!get_bit(index)) already_present = false;
            set_bit(index);
        }

        if (!already_present) count_++;
        return already_present;
   }


private:
    uint8_t *data_;
    size_t num_bits_;
    size_t num_hashes_;
    size_t count_;
    size_t checkpoint_rate_;
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
