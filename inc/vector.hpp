#pragma once

#include "compiler.hpp"
#include <stddef.h>
#include <stdint.h>

// Non-allocating vector with fixed capacity
template <typename T, size_t Capacity>
class Vector
{
  public:
    Vector() : size_(0) {}

    // Element access
    T &operator[](size_t index)
    {
        return data_[index];
    }

    const T &operator[](size_t index) const
    {
        return data_[index];
    }

    T &at(size_t index)
    {
        assert_truth(index < size_ && "Vector index out of bounds");
        return data_[index];
    }

    const T &at(size_t index) const
    {
        assert_truth(index < size_ && "Vector index out of bounds");
        return data_[index];
    }

    T &front()
    {
        assert_truth(size_ > 0 && "Vector is empty");
        return data_[0];
    }

    const T &front() const
    {
        assert_truth(size_ > 0 && "Vector is empty");
        return data_[0];
    }

    T &back()
    {
        assert_truth(size_ > 0 && "Vector is empty");
        return data_[size_ - 1];
    }

    const T &back() const
    {
        assert_truth(size_ > 0 && "Vector is empty");
        return data_[size_ - 1];
    }

    T *data()
    {
        return data_;
    }

    const T *data() const
    {
        return data_;
    }

    // Iterators
    T *begin()
    {
        return data_;
    }

    const T *begin() const
    {
        return data_;
    }

    T *end()
    {
        return data_ + size_;
    }

    const T *end() const
    {
        return data_ + size_;
    }

    // Capacity
    bool empty() const
    {
        return size_ == 0;
    }

    size_t size() const
    {
        return size_;
    }

    size_t capacity() const
    {
        return Capacity;
    }

    bool full() const
    {
        return size_ == Capacity;
    }

    // Modifiers
    void clear()
    {
        size_ = 0;
    }

    void push_back(const T &value)
    {
        assert_truth(size_ < Capacity && "Vector capacity exceeded");
        data_[size_++] = value;
    }

    void push_back(T &&value)
    {
        assert_truth(size_ < Capacity && "Vector capacity exceeded");
        data_[size_++] = static_cast<T &&>(value);
    }

    template <typename... Args>
    T &emplace_back(Args &&...args)
    {
        assert_truth(size_ < Capacity && "Vector capacity exceeded");
        T *ptr = &data_[size_++];
        new (ptr) T(static_cast<Args &&>(args)...);
        return *ptr;
    }

    void pop_back()
    {
        assert_truth(size_ > 0 && "Vector is empty");
        size_--;
    }

    void resize(size_t new_size)
    {
        assert_truth(new_size <= Capacity && "Resize exceeds capacity");
        size_ = new_size;
    }

    void resize(size_t new_size, const T &value)
    {
        assert_truth(new_size <= Capacity && "Resize exceeds capacity");
        if (new_size > size_)
        {
            for (size_t i = size_; i < new_size; i++)
            {
                data_[i] = value;
            }
        }
        size_ = new_size;
    }

    // Erase element at iterator position
    T *erase(T *pos)
    {
        assert_truth(pos >= begin() && pos < end() && "Invalid iterator");

        // Shift elements left
        for (T *it = pos; it < end() - 1; ++it)
        {
            *it = *(it + 1);
        }

        size_--;
        return pos;
    }

    // Erase element at index
    void erase(size_t index)
    {
        assert_truth(index < size_ && "Index out of bounds");

        // Shift elements left
        for (size_t i = index; i < size_ - 1; i++)
        {
            data_[i] = data_[i + 1];
        }

        size_--;
    }

    // Erase range [first, last)
    T *erase(T *first, T *last)
    {
        assert_truth(first >= begin() && first <= end() && "Invalid iterator");
        assert_truth(last >= first && last <= end() && "Invalid iterator range");

        if (first == last)
            return first;

        size_t count = last - first;

        // Shift elements left
        for (T *it = first; it < end() - count; ++it)
        {
            *it = *(it + count);
        }

        size_ -= count;
        return first;
    }

    // Insert element before position
    T *insert(T *pos, const T &value)
    {
        assert_truth(size_ < Capacity && "Vector capacity exceeded");
        assert_truth(pos >= begin() && pos <= end() && "Invalid iterator");

        // Shift elements right
        for (T *it = end(); it > pos; --it)
        {
            *it = *(it - 1);
        }

        *pos = value;
        size_++;
        return pos;
    }

  private:
    T data_[Capacity];
    size_t size_;
};
