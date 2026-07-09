#pragma once
#include <cstddef>
#include <cmath>
#include "hash_utils.h"


class BloomFilter {
public:
   BloomFilter(float false_positive_rate, size_t expected_items) {
    size_t num_bits = (expected_items * std::log(false_positive_rate)) / (std::pow(std::log(2), 2));
    num_bytes = (num_bits + 7) / 8;
    num_hashes_ = num_bits / expected_items;
    data_ = new uint8_t[num_bytes]();
   }


private:
    uint8_t *data_;
    size_t num_bytes;
    size_t num_hashes_;
};
