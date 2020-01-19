#pragma once

#include <cstddef>
#include <iterator>

template <class T> class span
{
  public:
    constexpr span(T* begin, T* end) : start{begin}, last{end} {}
    constexpr span(T* begin, std::size_t size) : span{begin, begin + size} {}

    template <class ArrayType, std::size_t N>
    constexpr span(ArrayType (&array)[N]) : span{std::begin(array), std::end(array)}
    {
    }

    template <class Cont>
    constexpr span(Cont& container)
        : start{std::data(container)}, last{start + std::size(container)}
    {
    }

    constexpr T& operator[](std::size_t index) { return start[index]; }

    constexpr const T& operator[](std::size_t index) const { return start[index]; }

    constexpr T* begin() const { return start; }

    constexpr T* end() const { return last; }

    constexpr std::size_t size() const { return last - start; }

  private:
    T* start;
    T* last;
};
