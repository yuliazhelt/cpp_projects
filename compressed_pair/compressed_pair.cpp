#pragma once

#include <utility>
#include <type_traits>

template <typename T, std::size_t I, bool = std::is_empty_v<T> && !std::is_final_v<T>>
struct CompressedPairElement;

template <typename T, std::size_t I>
struct CompressedPairElement<T, I, false> {
private:
    T value_;

public:
    CompressedPairElement() : value_(T()){};

    CompressedPairElement(const T& val) : value_(val){};

    CompressedPairElement(T&& val) : value_(std::forward<T&&>(val)){};

    T& GetValue() {
        return value_;
    }

    const T& GetValue() const {
        return value_;
    }
};

template <typename T, std::size_t I>
struct CompressedPairElement<T, I, true> : public T {
public:
    CompressedPairElement() {
    }

    CompressedPairElement(const T&){};

    CompressedPairElement(T&&){};

    T& GetValue() {
        return *this;
    }

    const T& GetValue() const {
        return *this;
    }
};

template <typename F, typename S>
class CompressedPair : private CompressedPairElement<F, 0>, private CompressedPairElement<S, 1> {
    using First = CompressedPairElement<F, 0>;
    using Second = CompressedPairElement<S, 1>;

public:
    CompressedPair() = default;

    CompressedPair(const F& first, const S& second) : First(first), Second(second){};

    CompressedPair(F&& first, S&& second)
            : First(std::forward<F>(first)), Second(std::forward<S>(second)){};

    CompressedPair(const F& first, S&& second) : First(first), Second(std::forward<S>(second)){};

    CompressedPair(F&& first, const S& second) : First(std::forward<F>(first)), Second(second){};

    F& GetFirst() {
        return First::GetValue();
    }

    const F& GetFirst() const {
        return First::GetValue();
    }

    S& GetSecond() {
        return Second ::GetValue();
    }

    const S& GetSecond() const {
        return Second ::GetValue();
    }
};