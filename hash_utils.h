#pragma once
#include <cstddef>
#include <cstdint>


inline uint64_t fnv1a_64(const void* data, size_t len) {
    constexpr uint64_t basis = 14695981039346656037ull;
    constexpr uint64_t prime = 1099511628211ull;
    const auto* bytes = static_cast<const uint8_t*>(data);
    uint64_t h = basis;
    for (size_t i = 0; i < len; ++i)
        h = (h ^ bytes[i]) * prime;
    return h;
}

template <typename T>
inline uint64_t fnv1a(const T& val) {
    return fnv1a_64(&val, sizeof(T));
}
