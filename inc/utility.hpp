#pragma once

template <typename T>
constexpr T &&move(T &&a)
{
    return static_cast<T &&>(a);
}

template <class T>
constexpr void swap(T &a, T &b)
{
    T tmp = move(a);
    a = move(b);
    b = move(tmp);
}
