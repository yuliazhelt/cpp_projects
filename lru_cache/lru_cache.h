#pragma once

#include <string>
#include <list>
#include <unordered_map>

class LruCache {
public:
    LruCache(size_t max_size);

    void Set(const std::string& key, const std::string& value);

    bool Get(const std::string& key, std::string* value);

    std::unordered_map<std::string, std::list<std::string>::iterator> key_iterator;
    std::unordered_map<std::string, std::string> key_value;
    std::list<std::string> list_;
    size_t max_size;
};