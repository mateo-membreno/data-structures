#include <cassert>
#include <functional>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include "sparse_dense_map.hpp"
#include "vector.h"
#include "mutex_vector.h"
#include "queue.h"
#include "priority_queue.h"
#include "unordered_set.h"
#include "merkle_tree.h"

void test_insert_and_get() {
    SparseDenseMap<std::string, int> m;
    m.insert("a", 1);
    m.insert("b", 2);
    assert(m.get("a") == 1);
    assert(m.get("b") == 2);
    std::cout << "PASS test_insert_and_get\n";
}

void test_update_existing_key() {
    SparseDenseMap<std::string, int> m;
    m.insert("a", 1);
    m.insert("a", 99);
    assert(m.get("a") == 99);
    std::cout << "PASS test_update_existing_key\n";
}

void test_contains() {
    SparseDenseMap<std::string, int> m;
    m.insert("x", 10);
    assert(m.contains("x") == true);
    assert(m.contains("y") == false);
    std::cout << "PASS test_contains\n";
}

void test_get_missing_throws() {
    SparseDenseMap<std::string, int> m;
    bool threw = false;
    try {
        m.get("missing");
    } catch (const std::out_of_range&) {
        threw = true;
    }
    assert(threw);
    std::cout << "PASS test_get_missing_throws\n";
}

void test_resize() {
    SparseDenseMap<int, int> m(4);
    for (int i = 0; i < 20; i++)
        m.insert(i, i * 10);
    for (int i = 0; i < 20; i++)
        assert(m.get(i) == i * 10);
    std::cout << "PASS test_resize\n";
}

void test_collision_handling() {
    SparseDenseMap<int, int> m(2);
    m.insert(0, 100);
    m.insert(2, 200);
    assert(m.get(0) == 100);
    assert(m.get(2) == 200);
    std::cout << "PASS test_collision_handling\n";
}

void test_remove_basic() {
    SparseDenseMap<std::string, int> m;
    m.insert("a", 1);
    m.remove("a");
    assert(m.contains("a") == false);
    bool threw = false;
    try { m.get("a"); } catch (const std::out_of_range&) { threw = true; }
    assert(threw);
    std::cout << "PASS test_remove_basic\n";
}

void test_remove_nonexistent() {
    SparseDenseMap<std::string, int> m;
    m.insert("a", 1);
    m.remove("ghost");   // should not crash or corrupt
    assert(m.get("a") == 1);
    std::cout << "PASS test_remove_nonexistent\n";
}

// Removing the last element inserted avoids the swap path (dense_idx == last_idx)
void test_remove_last_element() {
    SparseDenseMap<std::string, int> m;
    m.insert("a", 1);
    m.insert("b", 2);
    m.remove("b");
    assert(m.contains("b") == false);
    assert(m.get("a") == 1);
    std::cout << "PASS test_remove_last_element\n";
}

// Removing a middle element triggers the swap; the moved element must still be reachable
void test_remove_middle_element_sparse_update() {
    SparseDenseMap<std::string, int> m;
    m.insert("a", 1);
    m.insert("b", 2);
    m.insert("c", 3);
    m.remove("a");                 // "c" is swapped into "a"'s dense slot
    assert(m.contains("a") == false);
    assert(m.get("b") == 2);
    assert(m.get("c") == 3);      // fails if sparse_[c] wasn't updated
    std::cout << "PASS test_remove_middle_element_sparse_update\n";
}

void test_remove_then_reinsert() {
    SparseDenseMap<std::string, int> m;
    m.insert("a", 1);
    m.remove("a");
    m.insert("a", 42);
    assert(m.get("a") == 42);
    std::cout << "PASS test_remove_then_reinsert\n";
}

void test_remove_all_elements() {
    SparseDenseMap<std::string, int> m;
    m.insert("a", 1);
    m.insert("b", 2);
    m.insert("c", 3);
    m.remove("a");
    m.remove("b");
    m.remove("c");
    assert(m.contains("a") == false);
    assert(m.contains("b") == false);
    assert(m.contains("c") == false);
    std::cout << "PASS test_remove_all_elements\n";
}

void test_append_default_add() {
    SparseDenseMap<std::string, int> m;
    assert(m.append("a", 1) == 1);   // new key, returns inserted value
    assert(m.append("a", 4) == 5);   // existing key, 1+4=5
    assert(m.append("a", 10) == 15); // 5+10=15
    assert(m.get("a") == 15);
    std::cout << "PASS test_append_default_add\n";
}

void test_append_new_key_no_combine() {
    SparseDenseMap<std::string, int> m;
    // key didn't exist, so combine is never called — just inserts
    assert(m.append("x", 99) == 99);
    assert(m.get("x") == 99);
    std::cout << "PASS test_append_new_key_no_combine\n";
}

void test_append_custom_combine() {
    // use a custom combiner: keep the max
    SparseDenseMap<std::string, int> m;
    auto keep_max = [](int a, int b){ return a > b ? a : b; };
    m.append("k", 3, keep_max);
    m.append("k", 7, keep_max);
    m.append("k", 2, keep_max);
    assert(m.get("k") == 7);
    std::cout << "PASS test_append_custom_combine\n";
}

void test_append_string_concat() {
    SparseDenseMap<std::string, std::string> m;
    m.append("k", std::string("hello"));
    m.append("k", std::string(" world"));
    assert(m.get("k") == "hello world");
    std::cout << "PASS test_append_string_concat\n";
}

// Insert after removes to exercise tombstone probing
void test_insert_after_removes() {
    SparseDenseMap<int, int> m(4);
    for (int i = 0; i < 8; i++)
        m.insert(i, i);
    for (int i = 0; i < 4; i++)
        m.remove(i);
    for (int i = 8; i < 12; i++)
        m.insert(i, i);
    for (int i = 4; i < 12; i++)
        assert(m.get(i) == i);
    std::cout << "PASS test_insert_after_removes\n";
}

// ---- Vector tests ----

void test_vector_push_and_access() {
    Vector<int> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    assert(v[0] == 1);
    assert(v[1] == 2);
    assert(v[2] == 3);
    assert(v.size() == 3);
    std::cout << "PASS test_vector_push_and_access\n";
}

void test_vector_pop_back() {
    Vector<int> v;
    v.push_back(10);
    v.push_back(20);
    v.pop_back();
    assert(v.size() == 1);
    assert(v[0] == 10);
    std::cout << "PASS test_vector_pop_back\n";
}

void test_vector_pop_empty() {
    Vector<int> v;
    v.pop_back();  // should not crash or underflow
    assert(v.size() == 0);
    std::cout << "PASS test_vector_pop_empty\n";
}

void test_vector_fill_constructor() {
    Vector<int> v(5, 7);
    assert(v.size() == 5);
    for (size_t i = 0; i < 5; i++) assert(v[i] == 7);
    std::cout << "PASS test_vector_fill_constructor\n";
}

void test_vector_grow_past_capacity() {
    Vector<int> v;
    for (int i = 0; i < 20; i++) v.push_back(i);
    assert(v.size() == 20);
    for (int i = 0; i < 20; i++) assert(v[i] == i);
    std::cout << "PASS test_vector_grow_past_capacity\n";
}

void test_vector_remove_middle() {
    Vector<int> v;
    v.push_back(1); v.push_back(2); v.push_back(3); v.push_back(4);
    v.remove(1);  // remove 2
    assert(v.size() == 3);
    assert(v[0] == 1);
    assert(v[1] == 3);
    assert(v[2] == 4);
    std::cout << "PASS test_vector_remove_middle\n";
}

void test_vector_resize_grow() {
    Vector<int> v;
    v.push_back(1); v.push_back(2);
    v.resize(5);
    assert(v.size() == 5);
    assert(v[0] == 1);
    assert(v[1] == 2);
    assert(v[2] == 0);  // new slots default-initialized
    std::cout << "PASS test_vector_resize_grow\n";
}

void test_vector_resize_shrink() {
    Vector<int> v;
    v.push_back(1); v.push_back(2); v.push_back(3);
    v.resize(1);
    assert(v.size() == 1);
    assert(v[0] == 1);
    std::cout << "PASS test_vector_resize_shrink\n";
}

void test_vector_reserve() {
    Vector<int> v;
    v.push_back(1);
    v.reserve(100);
    assert(v.capacity() >= 100);
    assert(v.size() == 1);
    assert(v[0] == 1);
    std::cout << "PASS test_vector_reserve\n";
}

void test_vector_empty() {
    Vector<int> v;
    assert(v.empty());
    v.push_back(1);
    assert(!v.empty());
    std::cout << "PASS test_vector_empty\n";
}

// ---- MVector (mutex) tests ----

void test_mvector_push_and_access() {
    MVector<int> v;
    v.push_back(1);
    v.push_back(2);
    assert(v[0] == 1);
    assert(v[1] == 2);
    assert(v.size() == 2);
    std::cout << "PASS test_mvector_push_and_access\n";
}

void test_mvector_concurrent_push() {
    MVector<int> v;
    std::vector<std::thread> threads;
    for (int i = 0; i < 8; i++)
        threads.emplace_back([&v]{ for (int j = 0; j < 100; j++) v.push_back(j); });
    for (auto &t : threads) t.join();
    assert(v.size() == 800);
    std::cout << "PASS test_mvector_concurrent_push\n";
}

void test_mvector_concurrent_push_pop() {
    MVector<int> v;
    for (int i = 0; i < 100; i++) v.push_back(i);
    std::vector<std::thread> threads;
    for (int i = 0; i < 4; i++)
        threads.emplace_back([&v]{ for (int j = 0; j < 10; j++) v.push_back(j); });
    for (int i = 0; i < 4; i++)
        threads.emplace_back([&v]{ for (int j = 0; j < 10; j++) v.pop_back(); });
    for (auto &t : threads) t.join();
    // size should be 100 (40 pushed, 40 popped) — no crash is the main check
    assert(v.size() == 100);
    std::cout << "PASS test_mvector_concurrent_push_pop\n";
}

void test_mvector_remove() {
    MVector<int> v;
    v.push_back(10); v.push_back(20); v.push_back(30);
    v.remove(0);
    assert(v.size() == 2);
    assert(v[0] == 20);
    std::cout << "PASS test_mvector_remove\n";
}

// ---- Queue tests ----

void test_queue_default_constructor() {
    Queue<int> q;
    assert(q.size() == 0);
    std::cout << "PASS test_queue_default_constructor\n";
}

void test_queue_fill_constructor() {
    Queue<int> q(4, 7);
    assert(q.size() == 4);
    std::cout << "PASS test_queue_fill_constructor\n";
}

void test_queue_enqueue_size() {
    Queue<int> q;
    q.enqueue(1);
    q.enqueue(2);
    q.enqueue(3);
    assert(q.size() == 3);
    std::cout << "PASS test_queue_enqueue_size\n";
}

void test_queue_front_basic() {
    Queue<int> q;
    q.enqueue(10);
    q.enqueue(20);
    assert(q.front() == 10);
    std::cout << "PASS test_queue_front_basic\n" ;
}

void test_queue_dequeue_returns_front() {
    Queue<int> q;
    q.enqueue(1);
    q.enqueue(2);
    q.enqueue(3);
    assert(q.dequeue() == 1);
    assert(q.dequeue() == 2);
    assert(q.dequeue() == 3);
    std::cout << "PASS test_queue_dequeue_returns_front\n";
}

void test_queue_dequeue_updates_size() {
    Queue<int> q;
    q.enqueue(1);
    q.enqueue(2);
    q.dequeue();
    assert(q.size() == 1);
    assert(q.front() == 2);
    std::cout << "PASS test_queue_dequeue_updates_size\n";
}

void test_queue_grow_past_capacity() {
    Queue<int> q;
    for (int i = 0; i < 20; i++) q.enqueue(i);
    assert(q.size() == 20);
    assert(q.front() == 0);
    std::cout << "PASS test_queue_grow_past_capacity\n";
}

void test_queue_grow_after_dequeue() {
    Queue<int> q;
    for (int i = 0; i < 5; i++) q.enqueue(i);
    for (int i = 0; i < 3; i++) q.dequeue();  // head wraps around
    for (int i = 5; i < 15; i++) q.enqueue(i);  // triggers resize mid-wrap
    assert(q.size() == 12);
    assert(q.front() == 3);
    std::cout << "PASS test_queue_grow_after_dequeue\n";
}

// ---- PriorityQueue tests ----

void test_pq_max_heap_order() {
    PriorityQueue<int> pq;
    pq.push(3); pq.push(1); pq.push(4); pq.push(1); pq.push(5);
    assert(pq.top() == 5);
    pq.pop();
    assert(pq.top() == 4);
    pq.pop();
    assert(pq.top() == 3);
    std::cout << "PASS test_pq_max_heap_order\n";
}

void test_pq_min_heap_order() {
    PriorityQueue<int, std::greater<int>> pq;
    pq.push(3); pq.push(1); pq.push(4); pq.push(1); pq.push(5);
    assert(pq.top() == 1);
    pq.pop();
    assert(pq.top() == 1);
    pq.pop();
    assert(pq.top() == 3);
    std::cout << "PASS test_pq_min_heap_order\n";
}

void test_pq_size_and_empty() {
    PriorityQueue<int> pq;
    assert(pq.empty());
    assert(pq.size() == 0);
    pq.push(10);
    assert(!pq.empty());
    assert(pq.size() == 1);
    pq.pop();
    assert(pq.empty());
    std::cout << "PASS test_pq_size_and_empty\n";
}

void test_pq_pop_empty() {
    PriorityQueue<int> pq;
    pq.pop();  // should not crash
    assert(pq.size() == 0);
    std::cout << "PASS test_pq_pop_empty\n";
}

void test_pq_grow_past_capacity() {
    PriorityQueue<int> pq;
    for (int i = 0; i < 20; i++) pq.push(i);
    assert(pq.size() == 20);
    assert(pq.top() == 19);
    std::cout << "PASS test_pq_grow_past_capacity\n";
}

void test_pq_sorted_extraction() {
    PriorityQueue<int> pq;
    pq.push(5); pq.push(2); pq.push(8); pq.push(1); pq.push(9);
    int prev = pq.top(); pq.pop();
    while (!pq.empty()) {
        assert(pq.top() <= prev);
        prev = pq.top();
        pq.pop();
    }
    std::cout << "PASS test_pq_sorted_extraction\n";
}

void test_pq_custom_comparator() {
    // min-heap via lambda on a struct field
    struct Task { int priority; std::string name; };
    auto cmp = [](const Task& a, const Task& b){ return a.priority > b.priority; };
    PriorityQueue<Task, decltype(cmp)> pq(cmp);
    pq.push({3, "low"});
    pq.push({1, "urgent"});
    pq.push({2, "medium"});
    assert(pq.top().priority == 1);
    pq.pop();
    assert(pq.top().priority == 2);
    std::cout << "PASS test_pq_custom_comparator\n";
}

// ---- UnorderedSet tests ----

void test_uset_constructor_empty() {
    UnorderedSet<int> s;
    assert(s.size() == 0);
    assert(s.contains(1) == false);
    std::cout << "PASS test_uset_constructor_empty\n";
}

void test_uset_insert_and_contains() {
    UnorderedSet<int> s;
    s.insert(10);
    s.insert(20);
    assert(s.contains(10) == true);
    assert(s.contains(20) == true);
    assert(s.contains(99) == false);
    std::cout << "PASS test_uset_insert_and_contains\n";
}

void test_uset_insert_duplicate() {
    UnorderedSet<int> s;
    s.insert(5);
    s.insert(5);
    assert(s.size() == 1);
    std::cout << "PASS test_uset_insert_duplicate\n";
}

void test_uset_insert_triggers_resize() {
    UnorderedSet<int> s;
    for (int i = 0; i < 20; i++) s.insert(i);
    for (int i = 0; i < 20; i++) assert(s.contains(i) == true);
    assert(s.contains(99) == false);
    std::cout << "PASS test_uset_insert_triggers_resize\n";
}

void test_uset_remove_basic() {
    UnorderedSet<int> s;
    s.insert(7);
    s.remove(7);
    assert(s.contains(7) == false);
    assert(s.size() == 0);
    std::cout << "PASS test_uset_remove_basic\n";
}

void test_uset_remove_nonexistent() {
    UnorderedSet<int> s;
    s.insert(1);
    s.remove(99); // should not crash or corrupt
    assert(s.contains(1) == true);
    assert(s.size() == 1);
    std::cout << "PASS test_uset_remove_nonexistent\n";
}

void test_uset_remove_then_reinsert() {
    UnorderedSet<int> s;
    s.insert(3);
    s.remove(3);
    s.insert(3);
    assert(s.contains(3) == true);
    assert(s.size() == 1);
    std::cout << "PASS test_uset_remove_then_reinsert\n";
}

// Tombstone probing: element that probed past a deleted slot must still be found
void test_uset_contains_after_tombstone() {
    UnorderedSet<int> s;
    for (int i = 0; i < 10; i++) s.insert(i);
    s.remove(0);
    for (int i = 1; i < 10; i++) assert(s.contains(i) == true);
    std::cout << "PASS test_uset_contains_after_tombstone\n";
}

// ---- MerkleTree tests ----

void test_merkle_single_element() {
    std::vector<int> data = {42};
    MerkleTree tree(data.begin(), data.end());
    Hash256 expected = sha256(&data[0], sizeof(int));
    assert(tree.root() == expected);
    std::cout << "PASS test_merkle_single_element\n";
}

void test_merkle_two_elements() {
    std::vector<int> data = {1, 2};
    MerkleTree tree(data.begin(), data.end());
    Hash256 leaf0 = sha256(&data[0], sizeof(int));
    Hash256 leaf1 = sha256(&data[1], sizeof(int));
    Hash256 pair[2] = {leaf0, leaf1};
    Hash256 expected = sha256(pair, sizeof(pair));
    assert(tree.root() == expected);
    std::cout << "PASS test_merkle_two_elements\n";
}

void test_merkle_different_data_different_root() {
    std::vector<int> a = {1, 2, 3, 4};
    std::vector<int> b = {1, 2, 3, 5};
    MerkleTree ta(a.begin(), a.end());
    MerkleTree tb(b.begin(), b.end());
    assert(ta.root() != tb.root());
    std::cout << "PASS test_merkle_different_data_different_root\n";
}

void test_merkle_same_data_same_root() {
    std::vector<int> a = {10, 20, 30};
    std::vector<int> b = {10, 20, 30};
    MerkleTree ta(a.begin(), a.end());
    MerkleTree tb(b.begin(), b.end());
    assert(ta.root() == tb.root());
    std::cout << "PASS test_merkle_same_data_same_root\n";
}

void test_merkle_order_matters() {
    std::vector<int> a = {1, 2};
    std::vector<int> b = {2, 1};
    MerkleTree ta(a.begin(), a.end());
    MerkleTree tb(b.begin(), b.end());
    assert(ta.root() != tb.root());
    std::cout << "PASS test_merkle_order_matters\n";
}

void test_merkle_non_power_of_two() {
    // 3 leaves — tree pads to 4, last leaf is duplicated
    std::vector<int> data = {7, 8, 9};
    MerkleTree tree(data.begin(), data.end());
    // just verify it doesn't crash and root is deterministic
    MerkleTree tree2(data.begin(), data.end());
    assert(tree.root() == tree2.root());
    std::cout << "PASS test_merkle_non_power_of_two\n";
}

void test_merkle_large_dataset() {
    std::vector<int> data(128);
    for (int i = 0; i < 128; i++) data[i] = i;
    MerkleTree tree(data.begin(), data.end());
    MerkleTree tree2(data.begin(), data.end());
    assert(tree.root() == tree2.root());
    // mutate one element — root must differ
    data[64] = 999;
    MerkleTree tree3(data.begin(), data.end());
    assert(tree.root() != tree3.root());
    std::cout << "PASS test_merkle_large_dataset\n";
}

int main() {
    test_insert_and_get();
    test_update_existing_key();
    test_contains();
    test_get_missing_throws();
    test_resize();
    test_collision_handling();
    test_remove_basic();
    test_remove_nonexistent();
    test_remove_last_element();
    test_remove_middle_element_sparse_update();
    test_remove_then_reinsert();
    test_remove_all_elements();
    test_insert_after_removes();
    test_append_default_add();
    test_append_new_key_no_combine();
    test_append_custom_combine();
    test_append_string_concat();
    test_vector_push_and_access();
    test_vector_pop_back();
    test_vector_pop_empty();
    test_vector_fill_constructor();
    test_vector_grow_past_capacity();
    test_vector_remove_middle();
    test_vector_resize_grow();
    test_vector_resize_shrink();
    test_vector_reserve();
    test_vector_empty();
    test_mvector_push_and_access();
    test_mvector_concurrent_push();
    test_mvector_concurrent_push_pop();
    test_mvector_remove();
    test_queue_default_constructor();
    test_queue_fill_constructor();
    test_queue_enqueue_size();
    test_queue_front_basic();
    test_queue_dequeue_returns_front();
    test_queue_dequeue_updates_size();
    test_queue_grow_past_capacity();
    test_queue_grow_after_dequeue();
    test_pq_max_heap_order();
    test_pq_min_heap_order();
    test_pq_size_and_empty();
    test_pq_pop_empty();
    test_pq_grow_past_capacity();
    test_pq_sorted_extraction();
    test_pq_custom_comparator();
    test_uset_constructor_empty();
    test_uset_insert_and_contains();
    test_uset_insert_duplicate();
    test_uset_insert_triggers_resize();
    test_uset_remove_basic();
    test_uset_remove_nonexistent();
    test_uset_remove_then_reinsert();
    test_uset_contains_after_tombstone();
    test_merkle_single_element();
    test_merkle_two_elements();
    test_merkle_different_data_different_root();
    test_merkle_same_data_same_root();
    test_merkle_order_matters();
    test_merkle_non_power_of_two();
    test_merkle_large_dataset();
    std::cout << "All tests passed.\n";
    return 0;
}
