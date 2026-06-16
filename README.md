# Data Structures

A collection of hand-rolled C++ data structures.

---

## Sparse Dense Map

A hash map built on two arrays: a sparse index array (the hash table) that maps keys to positions, and a dense array that stores the actual key-value pairs packed together. Lookups go through the sparse layer for O(1) average access, while the dense array keeps live entries contiguous in memory so iterating over all entries is cache friendly — no skipping over empty buckets. This makes it a strong fit for workloads that mix full iteration with random lookups. Deletes tombstone sparse index and fill dense removed element with last element of vector.

---

## Vector

A generic array that grows automatically as elements are added. Backed by a heap allocated contiguous block of memory so random access is O(1) and iteration is cache friendly. When the array fills up, capacity doubles. Shrinking by `resize` or `remove` moves `size_` back without reallocating, elements past `size_` are still in memory until the vector is destroyed.
