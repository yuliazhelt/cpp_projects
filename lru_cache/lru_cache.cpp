#include "lru_cache.h"
#include <iostream>

LruCache::LruCache(size_t max_size) : max_size(max_size) {
}

void LruCache::Set(const std::string& key, const std::string& value) {
    list_.push_back(key);
    key_value[key] = value;
    key_iterator[key] = --list_.end();
    if (key_value.size() > max_size) {
        while (key_iterator[list_.front()] != list_.begin()) {
            list_.pop_front();
        }
        key_iterator.erase(list_.front());
        key_value.erase(list_.front());
        list_.pop_front();
    }
}

bool LruCache::Get(const std::string& key, std::string* value) {
    if (key_value.find(key) != key_value.end()) {
        list_.push_back(key);
        key_iterator[key] = --list_.end();
        *value = key_value[key];
        return true;
    }
    return false;
}