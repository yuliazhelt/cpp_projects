#pragma once


#include <cstddef>
#include <atomic>
#include <iostream>

class ControlBlockBase {
public:
    size_t links_num = 1;

    virtual ~ControlBlockBase() = default;
};

template <typename U>
class ControlBlockPointer : public ControlBlockBase {
public:
    U* ptr_;

    ControlBlockPointer(U* ptr) : ptr_(ptr) {
    }

    ~ControlBlockPointer() {
        delete ptr_;
    }
};

template <typename U>
class ControlBlockHolder : public ControlBlockBase {
public:
    template <typename... Args>
    ControlBlockHolder(Args&&... args) {
        new (&storage_) U(std::forward<Args>(args)...);
    }

    U* GetRaw() {
        return reinterpret_cast<U*>(&storage_);
    }

    ~ControlBlockHolder() {
        GetRaw()->~U();
    }

    std::aligned_storage_t<sizeof(U), alignof(U)> storage_;
};

template <typename T>
class SharedPtr {
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    SharedPtr() : sh_ptr(), block_() {
    }
    SharedPtr(std::nullptr_t) noexcept : sh_ptr(), block_() {
    }
    template <typename U>
    explicit SharedPtr(U* ptr) : sh_ptr(ptr), block_(new ControlBlockPointer<U>(ptr)) {
    }

    SharedPtr(T* ptr, ControlBlockBase* block) noexcept {
        sh_ptr = ptr;
        block_ = block;
    }

    SharedPtr(const SharedPtr& other) noexcept : sh_ptr(other.sh_ptr), block_(other.block_) {
        if (block_ != nullptr) {
            ++block_->links_num;
        }
    }

    template <typename U>
    SharedPtr(const SharedPtr<U>& other) noexcept : sh_ptr(other.sh_ptr), block_(other.block_) {
        if (block_ != nullptr) {
            ++block_->links_num;
        }
    }
    SharedPtr(SharedPtr&& other) noexcept
            : sh_ptr(std::move(other.sh_ptr)), block_(std::move(other.block_)) {
        other.sh_ptr = nullptr;
        other.block_ = nullptr;
    }

    template <typename Y>
    SharedPtr(SharedPtr<Y>&& other) noexcept
            : sh_ptr(std::move(other.sh_ptr)), block_(std::move(other.block_)) {
        other.sh_ptr = nullptr;
        other.block_ = nullptr;
    }

    // Aliasing constructor
    template <typename Y>
    SharedPtr(const SharedPtr<Y>& other, T* ptr) : sh_ptr(ptr), block_(other.block_) {
        ++block_->links_num;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s

    SharedPtr& operator=(const SharedPtr& other) noexcept {
        SharedPtr<T>(other).Swap(*this);
        return *this;
    }

    template <typename Y>
    SharedPtr& operator=(const SharedPtr<Y>& other) noexcept {
        SharedPtr<T>(other).Swap(*this);
        return *this;
    }

    SharedPtr& operator=(SharedPtr&& other) noexcept {
        SharedPtr<T>{std::move(other)}.Swap(*this);
        other.sh_ptr = nullptr;
        other.block_ = nullptr;
        return *this;
    }

    template <typename Y>
    SharedPtr& operator=(SharedPtr<Y>&& other) noexcept {
        SharedPtr<T>{std::move(other)}.Swap(*this);
        other.sh_ptr = nullptr;
        other.block_ = nullptr;
        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~SharedPtr() noexcept {
        if (block_ != nullptr) {
            if (--block_->links_num == 0) {
                delete block_;
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    void Reset() noexcept {
        SharedPtr<T>().Swap(*this);
    }

    void Reset(T* ptr) noexcept {
        SharedPtr<T>(ptr).Swap(*this);
    }

    template <typename U>
    void Reset(U* ptr) noexcept {
        SharedPtr<T>(ptr).Swap(*this);
    }

    void Swap(SharedPtr& other) noexcept {
        std::swap(sh_ptr, other.sh_ptr);
        std::swap(block_, other.block_);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    T* Get() const {
        return sh_ptr;
    }
    T& operator*() const {
        return *sh_ptr;
    }
    T* operator->() const {
        return sh_ptr;
    }
    size_t UseCount() const {
        if (block_ == nullptr) {
            return 0;
        }
        return block_->links_num;
    }
    explicit operator bool() const noexcept {
        return sh_ptr != nullptr;
    }

    T* sh_ptr;
    ControlBlockBase* block_;

    template <typename... Args>
    friend SharedPtr MakeShared(Args&&... args);
};

template <typename T, typename U>
inline bool operator==(const SharedPtr<T>& left, const SharedPtr<U>& right);

template <typename T, typename... Args>
SharedPtr<T> MakeShared(Args&&... args) {
    ControlBlockHolder<T>* block = new ControlBlockHolder<T>(std::forward<Args>(args)...);
    return SharedPtr<T>(block->GetRaw(), block);
}