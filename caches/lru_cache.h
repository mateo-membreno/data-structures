#include "vector.h"

template <typename K, typename V, typename Hash = std::hash<K>>
class LRUCache {
    
public:
    LRUCache(size_t size) : size_(size), used_(0){
        cache_.resize(size_, nullptr);
    }

    void insert(const K& key, const V& value){
        if (size_ == used_) {
            evict();
        }
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

    size_t get_index(Entry* entry){
        return std::hash<K>{}(entry->key) % size_;
    }

    void unlink_entry(Entry* entry){
        size_t index = get_index(entry);
        Entry* curr = cache_[index];
        Entry* prev = nullptr;
        while (curr != entry){
            prev = curr;
            curr = curr->hash_next;
        }
        if (prev == nullptr){
            cache_[index] = curr->hash_next;
        } else {
            prev->hash_next = curr->hash_next;
        }
    }


    void evict(){
        Entry* victim = tail_;
        if (victim == nullptr) return;
        unlink_entry(tail_);

        tail_ = victim->prev;
        if (tail_ != nullptr){
            tail_->next = nullptr;
        } else {
            head_ = nullptr;
        }
        delete victim;
        used--;

    };

};