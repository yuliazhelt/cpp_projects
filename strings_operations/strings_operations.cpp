#include "string_operations.h"
#include <iostream>
#include <filesystem>

bool StartsWith(std::string_view string, std::string_view text) {
    return string.starts_with(text);
}

bool EndsWith(std::string_view string, std::string_view text) {
    return string.ends_with(text);
}

std::string_view StripPrefix(std::string_view string, std::string_view prefix) {
    if (StartsWith(string, prefix)) {
        string.remove_prefix(prefix.size());
    }
    return string;
}

std::string_view StripSuffix(std::string_view string, std::string_view suffix) {
    if (EndsWith(string, suffix)) {
        string.remove_suffix(suffix.size());
    }
    return string;
}

std::string_view ClippedSubstr(std::string_view s, size_t pos, size_t n) {
    if (n > s.size()) {
        return s;
    }
    return s.substr(pos, n);
}

std::string_view StripAsciiWhitespace(std::string_view string) {
    size_t j = 0;
    while (j < string.size() && std::isspace(string[j])) {
        ++j;
    }
    size_t i = string.size() - 1;
    while (i > j && std::isspace(string[i])) {
        --i;
    }
    return string.substr(j, i - j + 1);
}

std::string AddSlash(std::string_view path) {
    if (path.back() == '/') {
        return std::string(path);
    }
    std::string s;
    s.reserve(path.size() + 1);
    for (const char& c : path) {
        s.push_back(c);
    }
    s.push_back('/');
    return s;
}

std::string_view RemoveSlash(std::string_view path) {
    if (path.back() == '/' && path != "/") {
        return StripSuffix(path, "/");
    }
    return path;
}

std::string_view Dirname(std::string_view path) {
    size_t pos = path.find_last_of('/');
    if (pos != 0) {
        return path.substr(0, pos);
    }
    return "/";
}

std::string_view Basename(std::string_view path) {
    size_t pos = path.find_last_of('/');
    return path.substr(pos + 1, path.size() - pos);
}

std::string CollapseSlashes(std::string_view path) {
    size_t capacity = 0;
    for (size_t i = 1; i < path.size(); ++i) {
        if (path[i] != '/' || path[i - 1] != '/') {
            ++capacity;
        }
    }
    std::string ans;
    ans.reserve(capacity + 1);
    for (size_t i = 0; i < path.size(); ++i) {
        if (i == 0) {
            ans.push_back(path[i]);
        } else if (path[i] != '/' || path[i - 1] != '/') {
            ans.push_back(path[i]);
        }
    }
    return ans;
}

std::string StrJoin(const std::vector<std::string_view>& strings, std::string_view delimiter) {
    if (strings.empty()) {
        return "";
    }
    size_t capacity = 0;
    for (auto str : strings) {
        capacity += str.size();
    }
    capacity += (strings.size() - 1) * delimiter.size();
    std::string ans;
    ans.reserve(capacity);
    for (size_t i = 0; i < strings.size(); ++i) {
        for (auto c : strings[i]) {
            ans.push_back(c);
        }
        if (i != strings.size() - 1) {
            for (auto c : delimiter) {
                ans.push_back(c);
            }
        }
    }
    return ans;
}

std::vector<std::string_view> StrSplit(std::string_view text, std::string_view delim) {
    size_t pos = 0;
    for (size_t i = 0; i < text.size(); ++i) {
        if (text.substr(i, delim.size()) == delim) {
            ++pos;
        }
    }
    std::vector<std::string_view> ans;
    ans.reserve(pos + 1);
    size_t prev_pos = 0;
    for (size_t i = 0; i < text.size(); ++i) {
        if (text.substr(i, delim.size()) == delim) {
            ans.push_back(text.substr(prev_pos, i - prev_pos));
            prev_pos = i + delim.size();
        }
    }
    ans.push_back(text.substr(prev_pos, text.size() - prev_pos + 1));
    return ans;
}

std::string ReadN(const std::string& filename, size_t n) {
    std::string ans(n, '0');
    ssize_t flag = open(filename.c_str(), O_RDONLY);
    if (flag == -1) {
        ans.clear();
        return "";
    }
    size_t pos = 0;
    ssize_t b = 0;
    do {
        b = read(flag, ans.data() + pos, n - pos);
        if (b == -1) {
            ans.clear();
            return "";
        }
        pos += b;
    } while (pos < n);
    return ans;
}