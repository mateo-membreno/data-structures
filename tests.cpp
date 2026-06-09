#include <cassert>
#include <iostream>
#include <string>
#include "sparse_dense_map.hpp"

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
    std::cout << "All tests passed.\n";
    return 0;
}
