#pragma once

#include "compiler.hpp"

namespace Atomic
{
    enum memory_order : int {
        relaxed = __ATOMIC_RELAXED,
        consume = __ATOMIC_CONSUME,
        acquire = __ATOMIC_ACQUIRE,
        release = __ATOMIC_RELEASE,
        acq_rel = __ATOMIC_ACQ_REL,
        seq_cst = __ATOMIC_SEQ_CST
    };

    template <typename T>
    struct storage {
        T value{};
    };

    template <typename T>
    INLINE auto load(T *ptr, memory_order order)
    {
        return __atomic_load_n(ptr, order);
    }

    template <typename T>
    INLINE void store(T *ptr, T *val, memory_order order)
    {
        __atomic_store(ptr, val, order);
    }

    template <typename T>
    INLINE void store(T *ptr, T val, memory_order order)
    {
        __atomic_store_n(ptr, val, order);
    }

    template <typename T>
    INLINE void set(storage<T> &type, T value)
    {
        type.value = value;
    }

    template <typename T>
    INLINE T fetch_add(T *ptr, T val, memory_order order)
    {
        return __atomic_fetch_add(ptr, val, order);
    }

    template <typename T>
    INLINE T fetch_sub(T *ptr, T val, memory_order order)
    {
        return __atomic_fetch_sub(ptr, val, order);
    }

    template <typename T>
    struct impl {
        impl() = default;
        impl(T value) { Atomic::set(store, value); }

        // clang-format off
        T operator++() { return Atomic::fetch_add<T>(store.value, 1) + 1; }
        T operator++(int) { return Atomic::fetch_add<T>(&store.value, 1, memory_order::seq_cst); }
        T operator--(int) { return Atomic::fetch_sub<T>(&store.value, 1, memory_order::seq_cst); }
        T operator+=(int inc) { return Atomic::fetch_add<T>(&store.value, inc, memory_order::seq_cst); }
        T operator-=(int dec) { return Atomic::fetch_sub<T>(&store.value, dec, memory_order::seq_cst); }
        bool operator==(T v) { return store.value == v; }

        void operator=(T value) { Atomic::set(store, value); }
        // clang-format on

      private:
        storage<T> store;
    };
} // namespace Atomic

// Custom atomic type
template <typename T>
using atomic = Atomic::impl<T>;

using atomic_bool = Atomic::impl<bool>;
using atomic_char = Atomic::impl<char>;

using atomic_ushort = Atomic::impl<uint16_t>;
using atomic_short = Atomic::impl<int16_t>;

using atomic_uint = Atomic::impl<uint32_t>;
using atomic_int = Atomic::impl<int32_t>;
