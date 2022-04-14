#include <bit>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace big {
using size_type = std::size_t;

namespace detail {
template <size_type Bits>
struct underlying_int;

template <>
struct underlying_int<8> {
    using type = std::uint8_t;
};

template <>
struct underlying_int<16> {
    using type = std::uint16_t;
};

template <>
struct underlying_int<32> {
    using type = std::uint32_t;
};

template <>
struct underlying_int<64> {
    using type = std::uint64_t;
};

template <size_type Bits>
using underlying_int_t = typename underlying_int<Bits>::type;

template <size_type Bits, typename = void>
struct best_int {
    constexpr static size_type value =
        best_int<(static_cast<size_type>(1) << std::bit_width(Bits - 1)) / 2>::value;
};

template <>
struct best_int<0> {
    constexpr static size_type value = 8;
};

template <size_type Bits>
struct best_int<Bits,
                std::void_t<typename underlying_int<(static_cast<size_type>(1)
                                                     << std::bit_width(Bits - 1))>::type>> {
    constexpr static size_type value = static_cast<size_type>(1) << std::bit_width(Bits - 1);
};

template <size_type Bits>
constexpr static size_type best_int_v = best_int<Bits>::value;

}  // namespace detail

// Fixed width integer type with given underlying raw int and N copies of it.
// Takes up sizeof(Underlying)*N bytes of space.
// If N is zero, then this is a zero-size struct ([[no_unique_address]] will make it zero-size).
template <typename Underlying, size_type N>
struct basic_fixed_int {};

// Alias for basic_fixed_int with at least `Bits` bits (base 2).
// Will use less than twice as much space as necessary.
template <size_type Bits>
using fixed_int =
    basic_fixed_int<detail::underlying_int_t<detail::best_int_v<Bits>>,
                    (Bits + (detail::best_int_v<Bits> - 1)) / detail::best_int_v<Bits>>;
}  // namespace big
