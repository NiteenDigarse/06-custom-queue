#pragma once

#include <cstddef>
#include <memory>
#include <stdexcept>
#include <utility>

namespace custom {

template <typename T>
class CustomQueue {
private:
    T* data_;
    std::size_t capacity_;
    std::size_t front_;
    std::size_t back_;
    std::size_t size_;

    std::size_t back_index() const {
        return (back_ - 1 + capacity_) % capacity_;
    }

    void destroy_elements() noexcept {
        for (std::size_t i = 0; i < size_; ++i) {
            std::size_t index = (front_ + i) % capacity_;
            std::destroy_at(data_ + index);
        }
    }

public:
    explicit CustomQueue(std::size_t capacity)
        : data_(nullptr),
          capacity_(capacity),
          front_(0),
          back_(0),
          size_(0) {

        if (capacity == 0) {
            throw std::invalid_argument("Queue capacity cannot be zero");
        }

        data_ = static_cast<T*>(::operator new(sizeof(T) * capacity_));
    }

    ~CustomQueue() {
        if (data_) {
            destroy_elements();
            ::operator delete(data_);
        }
    }

    // Copy constructor
    CustomQueue(const CustomQueue& other)
        : data_(static_cast<T*>(
              ::operator new(sizeof(T) * other.capacity_))),
          capacity_(other.capacity_),
          front_(0),
          back_(0),
          size_(0) {

        try {
            for (std::size_t i = 0; i < other.size_; ++i) {
                std::size_t index =
                    (other.front_ + i) % other.capacity_;

                std::construct_at(
                    data_ + i,
                    other.data_[index]
                );

                ++size_;
                ++back_;
            }
        }
        catch (...) {
            destroy_elements();
            ::operator delete(data_);
            throw;
        }
    }

    // Copy assignment
    CustomQueue& operator=(const CustomQueue& other) {
        if (this == &other) {
            return *this;
        }

        CustomQueue temp(other);
        swap(temp);

        return *this;
    }

    // Move constructor
    CustomQueue(CustomQueue&& other) noexcept
        : data_(other.data_),
          capacity_(other.capacity_),
          front_(other.front_),
          back_(other.back_),
          size_(other.size_) {

        other.data_ = nullptr;
        other.capacity_ = 0;
        other.front_ = 0;
        other.back_ = 0;
        other.size_ = 0;
    }

    // Move assignment
    CustomQueue& operator=(CustomQueue&& other) noexcept {
        if (this == &other) {
            return *this;
        }

        if (data_) {
            destroy_elements();
            ::operator delete(data_);
        }

        data_ = other.data_;
        capacity_ = other.capacity_;
        front_ = other.front_;
        back_ = other.back_;
        size_ = other.size_;

        other.data_ = nullptr;
        other.capacity_ = 0;
        other.front_ = 0;
        other.back_ = 0;
        other.size_ = 0;

        return *this;
    }

    void swap(CustomQueue& other) noexcept {
        std::swap(data_, other.data_);
        std::swap(capacity_, other.capacity_);
        std::swap(front_, other.front_);
        std::swap(back_, other.back_);
        std::swap(size_, other.size_);
    }

    // Enqueue lvalue
    void enqueue(const T& value) {
        if (size_ == capacity_) {
            throw std::runtime_error("Queue is full");
        }

        std::construct_at(data_ + back_, value);

        back_ = (back_ + 1) % capacity_;
        ++size_;
    }

    // Enqueue rvalue
    void enqueue(T&& value) {
        if (size_ == capacity_) {
            throw std::runtime_error("Queue is full");
        }

        std::construct_at(data_ + back_, std::move(value));

        back_ = (back_ + 1) % capacity_;
        ++size_;
    }

    void dequeue() {
        if (size_ == 0) {
            throw std::runtime_error("Queue is empty");
        }

        std::destroy_at(data_ + front_);

        front_ = (front_ + 1) % capacity_;
        --size_;
    }

    T& front() {
        if (size_ == 0) {
            throw std::runtime_error("Queue is empty");
        }

        return data_[front_];
    }

    const T& front() const {
        if (size_ == 0) {
            throw std::runtime_error("Queue is empty");
        }

        return data_[front_];
    }

    T& back() {
        if (size_ == 0) {
            throw std::runtime_error("Queue is empty");
        }

        return data_[back_index()];
    }

    const T& back() const {
        if (size_ == 0) {
            throw std::runtime_error("Queue is empty");
        }

        return data_[back_index()];
    }

    bool empty() const {
        return size_ == 0;
    }

    std::size_t size() const {
        return size_;
    }
};

} // namespace custom