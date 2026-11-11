#include "vector.h"

template <typename K, typename V, typename Hash = std::hash<K>>
class LRUCache {
    
public:
    LRUCache(size_t size) : size_(size), used_(0){
        cache_.resize(size_, nullptr);
    }

    void insert(const K& key, const V& value){

    };

    void remove(const K& key){

    };

    V* get(const K& key){

    }


private:
    struct Entry{
        K key;
        V value;
        Entry* next = nullptr;
        Entry* prev = nullptr;
        Entry* hash_next = nullptr;
    };
 
    size_t size_;
    size_t used_;
    vector<Entry*> cache_;
    Entry* head_ = nullptr;
    Entry* tail_ = nullptr;

    void attach_to_front(Entry* entry){

    };

    void evict(){

    };

};