#pragma once
#include <string>
#include <string_view>
#include <vector>
#include <iostream>
#include <fstream>
#include <type_traits>
#include <climits>
#include <cstdint>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

bool StartsWith(std::string_view string, std::string_view text);

bool EndsWith(std::string_view string, std::string_view text);

std::string_view StripPrefix(std::string_view string, std::string_view prefix);

std::string_view StripSuffix(std::string_view string, std::string_view suffix);

std::string_view ClippedSubstr(std::string_view s, size_t pos, size_t n);

std::string_view StripAsciiWhitespace(std::string_view string);

std::string AddSlash(std::string_view path);

std::string_view RemoveSlash(std::string_view path);

std::string_view Dirname(std::string_view path);

std::string_view Basename(std::string_view path);

std::string CollapseSlashes(std::string_view path);

std::string StrJoin(const std::vector<std::string_view>& strings, std::string_view delimiter);

std::vector<std::string_view> StrSplit(std::string_view text, std::string_view delim);

std::string ReadN(const std::string& filename, size_t n);

inline size_t TemplSize(const std::string& s) {
    return s.size();
}

inline size_t TemplSize(std::string_view s) {
    return s.size();
}

inline size_t TemplSize(const char* s) {
    std::string_view t = s;
    return t.size();
}

template <typename T>
inline size_t TemplSize(T t) {
    size_t cnt = 0;
    if (t == T(0)) {
        return 1;
    }
    if (t < T(0)) {
        ++cnt;
        if (t % 10 != 0) {
            ++t;
        }
        t *= -1;
    }
    unsigned long long curr = 1;
    while (static_cast<unsigned long long>(t) >= curr && cnt <= 19) {
        curr *= 10;
        ++cnt;
    }
    if (static_cast<unsigned long long>(t) > curr && cnt == 19) {
        ++cnt;
    }
    return cnt;
}

template <class T>
inline void ToStr(std::string& ans, const T& n) {
    T k = n;
    size_t old_size = ans.size();
    size_t new_size = old_size + TemplSize(n);
    if (k == T(0)) {
        ans.push_back('0');
    } else if (k < T(0)) {
        while (k != 0) {
            char c = -(k % 10) + 48;
            ans.push_back(c);
            k /= 10;
        }
        ans.push_back('-');
    } else {
        while (k != 0) {
            char c = (k % 10) + 48;
            ans.push_back(c);
            k /= 10;
        }
    }
    std::reverse(ans.begin() + old_size, ans.begin() + new_size);
}

inline void ToStr(std::string& ans, const std::string& s) {
    for (const char& c : s) {
        ans.push_back(c);
    }
}

inline void ToStr(std::string& ans, std::string_view s) {
    for (const char& c : s) {
        ans.push_back(c);
    }
}

inline void ToStr(std::string& ans, const char* p) {
    std::string_view s = p;
    ToStr(ans, s);
}

template <typename T, typename... Params>
void MakeStr(std::string& ans, T&& t, Params&&... params) {
    ToStr(ans, t);
    (MakeStr(ans, std::forward<Params>(params)), ...);
}

template <typename T, typename... Params>
void CountSize(size_t& curr, T&& t, Params&&... params) {
    curr += TemplSize(t);
    (CountSize(curr, std::forward<Params>(params)), ...);
}

template <class... Params>
std::string StrCat(Params&&... params) {
    std::string ans;
    size_t all_size = 0;
    (CountSize(all_size, std::forward<Params>(params)), ...);
    ans.reserve(all_size);
    (MakeStr(ans, std::forward<Params>(params)), ...);
    return ans;
}