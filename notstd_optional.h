#pragma once

#include <stdexcept>
#include <utility>

namespace notstd {
    class BadOptionalAccess : public std::exception {
    public:
        using exception::exception;

        virtual const char* what() const noexcept override {
            return "Bad optional access";
        }
    };

    template <typename T>
    class Optional {
    public:
        Optional() = default;
        Optional(const T& value) {
            raw_ptr_ = new (&data_[0]) T(value);
            is_initialized_ = true;
        }
        Optional(T&& value) {
            raw_ptr_ = new (&data_[0]) T(std::move(value));
            is_initialized_ = true;
        }
        Optional(const Optional& other) {
            if (other.is_initialized_) {
                raw_ptr_ = new (&data_[0]) T(other.Value());
                is_initialized_ = true;
            }        
        }
        Optional(Optional&& other) {
            if (other.is_initialized_) {
                raw_ptr_ = new (&data_[0]) T(std::move(other.Value()));
                is_initialized_ = true;
            }        
        }

        Optional& operator=(const T& value) {
            if (is_initialized_) {
                *raw_ptr_ = value;
            } else {
                raw_ptr_ = new (&data_[0]) T(value);
                is_initialized_ = true;
            }
            return *this;
        }    
        Optional& operator=(T&& rhs) {
            if (is_initialized_) {
                *raw_ptr_ = std::move(rhs);
            } else {
                raw_ptr_ = new (&data_[0]) T(std::move(rhs));
                is_initialized_ = true;
            }
            return *this;
        }
        Optional& operator=(const Optional& rhs) {
            if (is_initialized_ && rhs.is_initialized_) {
                *raw_ptr_ = *(rhs.raw_ptr_);
            } else if (!is_initialized_ && rhs.is_initialized_) {
                raw_ptr_ = new (&data_[0]) T(*(rhs.raw_ptr_));
                is_initialized_ = true;
            } else if (is_initialized_ && !rhs.is_initialized_) {
                raw_ptr_->~T();
                is_initialized_ = false;
            } 

            return *this;
        }
        Optional& operator=(Optional&& rhs) {
            if (is_initialized_ && rhs.is_initialized_) {
                *raw_ptr_ = std::move(*(rhs.raw_ptr_));
            } else if (!is_initialized_ && rhs.is_initialized_) {
                raw_ptr_ = new (&data_[0]) T(std::move(*(rhs.raw_ptr_)));
                is_initialized_ = true;
            } else if (is_initialized_ && !rhs.is_initialized_) {
                raw_ptr_->~T();
                is_initialized_ = false;
            } 

            return *this;
        }

        ~Optional() {
            if (is_initialized_) {
                raw_ptr_->~T();
            }
            
        }

        bool HasValue() const {
            return is_initialized_;
        }

        T& operator*() & {
            return *raw_ptr_;
        }
        const T& operator*() const& {
            return *raw_ptr_;
        }
        T&& operator*() && {
            return std::move(*raw_ptr_);
        }
        T* operator->() {
            return raw_ptr_;
        }
        const T* operator->() const {
            return raw_ptr_;
        }
        
        T& Value() & {
            if (!is_initialized_) {
                throw BadOptionalAccess();
            }
            return *raw_ptr_;
        }
        const T& Value() const& {
            if (!is_initialized_) {
                throw BadOptionalAccess();
            }
            return *raw_ptr_;
        }
        T&& Value() && {
            if (!is_initialized_) {
                throw BadOptionalAccess();
            }
            return std::move(*raw_ptr_);
        }

        void Reset(){
            if (is_initialized_) {
                raw_ptr_->~T();
                is_initialized_ = false;
            }
        }
        
        template <typename... Ts>
        void Emplace(Ts&&... ts) {
            Reset();
            raw_ptr_ = new (&data_[0]) T(std::forward<Ts>(ts)...);
            is_initialized_ = true;
        }

    private:
        alignas(T) char data_[sizeof(T)];
        T* raw_ptr_;
        bool is_initialized_ = false;
    };
} //namespace notstd