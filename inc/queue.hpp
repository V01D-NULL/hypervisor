#pragma once

#include "compiler.hpp"
#include "trace.hpp"
#include "utility.hpp"

template <typename T>
class QueueNonAllocItem;

enum class QueueResult : uint8_t { ENQUEUE_FAIL, ENQUEUE_OKAY };

template <typename T>
class QueueNonAlloc
{
  private:
    static constexpr signed long limit = -1;
    size_t _size{};

    // [back] ||||||||| [front]
    T _back{nullptr}, _front{nullptr};

  public:
    INLINE QueueResult push(T &item)
    {
        if (item == nullptr || item->enqueued)
            return QueueResult::ENQUEUE_FAIL;

        // Did we impose a size limit on NonAllocQueues'?
        if constexpr (limit != -1)
            if (_size > limit)
                return QueueResult::ENQUEUE_FAIL;

        _size++;
        if (_back == nullptr)
        {
            _front = _back = item;
            return QueueResult::ENQUEUE_OKAY;
        }

        item->enqueued = true;
        _back->next = item;
        _back = item;

        return QueueResult::ENQUEUE_OKAY;
    }

    INLINE T pop()
    {
        if (empty())
            return T();

        auto node = _front;
        node->enqueued = false;
        _front = _front->next;

        if (!_front)
            _back = nullptr;

        return node;
    }

    INLINE size_t size() const { return _size; }
    INLINE bool empty() const { return _front == nullptr; }
    INLINE T front() const { return _front; }
    INLINE T back() const { return _back; }
};

template <typename T>
struct QueueNonAllocItem {
  public:
    T *next{nullptr};
    bool enqueued{false};
};