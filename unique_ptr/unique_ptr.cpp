#pragma once

#include <utility>

#include <cstddef>

struct Slug {
    template <typename T>
    void operator()(T* ptr) {
        delete ptr;
    }
};

template <typename T, typename Deleter = Slug>
class UniquePtr {
    using DelT = typename std::decay<Deleter>::type;

public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    explicit UniquePtr(T* ptr = nullptr) : ptr_(ptr) {
    }
    UniquePtr(T* ptr, const DelT& deleter) : ptr_(ptr), deleter_(deleter) {
    }

    UniquePtr(T* ptr, DelT& deleter) : ptr_(ptr), deleter_(deleter) {
    }

    UniquePtr(T* ptr, DelT&& deleter) : ptr_(ptr), deleter_(std::forward<Deleter>(deleter)) {
    }

    template <typename U, typename Del2>
    UniquePtr(UniquePtr<U, Del2>&& other) noexcept
            : ptr_(other.Release()), deleter_(std::forward<Deleter>(other.GetDeleter())) {
    }

    UniquePtr(UniquePtr& other) = delete;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s

    UniquePtr& operator=(UniquePtr&& other) noexcept {
        Reset(other.Release());
        deleter_ = std::forward<Deleter>(other.deleter_);
        return *this;
    }
    UniquePtr& operator=(std::nullptr_t) noexcept {
        Reset(nullptr);
        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~UniquePtr() noexcept {
        if (ptr_ != nullptr) {
            deleter_(ptr_);
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    T* Release() {
        T* copy = ptr_;
        ptr_ = nullptr;
        return copy;
    }

    void Reset(T* ptr = nullptr) noexcept {
        if (ptr_ != ptr) {
            auto old_ptr = ptr_;
            ptr_ = ptr;
            if (old_ptr) {
                deleter_(old_ptr);
            }
        }
    }

    void Swap(UniquePtr& other) noexcept {
        if (ptr_ != other.ptr_) {
            std::swap(ptr_, other.ptr_);
            std::swap(deleter_, other.deleter_);
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    T* Get() const noexcept {
        return ptr_;
    }
    Deleter& GetDeleter() {
        return deleter_;
    }
    const Deleter& GetDeleter() const {
        return deleter_;
    }
    explicit operator bool() const noexcept {
        return ptr_ != nullptr;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Single-object dereference operators

    typename std::add_lvalue_reference<T>::type operator*() const noexcept {
        return *ptr_;
    }
    T* operator->() const noexcept {
        return ptr_;
    }

private:
    [[no_unique_address]] T* ptr_;
    [[no_unique_address]] Deleter deleter_;
};

// Specialization for arrays
template <typename T, typename Deleter>
class UniquePtr<T[], Deleter> {
    using DelT = typename std::decay<Deleter>::type;

public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    explicit UniquePtr(T* ptr = nullptr) : ptr_(ptr) {
    }
    UniquePtr(T* ptr, const DelT& deleter) : ptr_(ptr), deleter_(deleter) {
    }

    UniquePtr(T* ptr, DelT& deleter) : ptr_(ptr), deleter_(deleter) {
    }

    UniquePtr(T* ptr, DelT&& deleter) : ptr_(ptr), deleter_(std::forward<Deleter>(deleter)) {
    }

    template <typename U, typename Del2>
    UniquePtr(UniquePtr<U, Del2>&& other) noexcept
            : ptr_(other.Release()), deleter_(std::forward<Deleter>(other.GetDeleter())) {
    }

    UniquePtr(UniquePtr& other) = delete;

    ~UniquePtr() noexcept {
        if (ptr_ != nullptr) {
            delete[] ptr_;
        }
    }

    void Reset(T* ptr = nullptr) noexcept {
        if (ptr_ != ptr) {
            auto old_ptr = ptr_;
            ptr_ = ptr;
            if (old_ptr) {
                delete[] old_ptr;
            }
        }
    }

    T& operator[](int i) const {
        return ptr_[i];
    }

private:
    [[no_unique_address]] T* ptr_;
    [[no_unique_address]] Deleter deleter_;
};