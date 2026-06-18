#pragma once
#include <cstddef>
#include "hash_utils.h"

class MerkleTree {
public:
    template <typename Iterator>
    MerkleTree(Iterator first, Iterator last) : data_(nullptr), size_(0) {
        int count = 0;
        for (auto it = first; it != last; ++it)
            count++;
        build(first, last, count);
    }

    ~MerkleTree() {
        delete[] data_;
    }

    Hash256 root() const {
        return data_[0];
    }


private:
    Hash256* data_;
    size_t size_;

    template <typename Iterator>
    void build(Iterator first, Iterator last, int count) {
        if (count == 0) return;

        int leaf_count = 1;
        while (leaf_count < count) {
            leaf_count <<= 1;
        }
        

        int total = 2 * leaf_count - 1;
        data_ = new Hash256[total];
        size_ = total;

        int leaf_start = leaf_count - 1;

        int i = leaf_start;
        for (auto it = first; it != last; it++, i++)
            data_[i] = sha256(&(*it), sizeof(*it));

        for (; i < total; i++)
            data_[i] = data_[i - 1];

        for (int j = leaf_start - 1; j >= 0; --j) {
            Hash256 pair[2] = { data_[2*j+1], data_[2*j+2] };
            data_[j] = sha256(pair, sizeof(pair));
        }
    }
};
