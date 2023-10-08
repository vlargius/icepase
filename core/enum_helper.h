#pragma once

#include <type_traits>

namespace {
template <int Value, int Bits> struct required_bits_helper {
    static constexpr int value = required_bits_helper<Value / 2, Bits + 1>::value;
};

template <int Bits> struct required_bits_helper<0, Bits> {
    static constexpr int value = Bits;
};
}   // namespace

namespace utils {

template <int Value> struct required_bits {
    static constexpr int value = required_bits_helper<Value, 0>::value;
};

}   // namespace utils