#include <algorithm>
#include <array>
#include <big/intrin.hpp>
#include <big/util.hpp>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace big {
using size_type = std::size_t;

namespace detail {
/// The predefined integer type with this number of bits.  If such type does not exist then this
/// type is not defined.
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

/// Fixed width integer type with given underlying raw int and N copies of it.
/// Takes up sizeof(Underlying)*N bytes of space.
/// If N is zero, then this is a zero-size struct ([[no_unique_address]] will make it zero-size).
/// This is an aggregate type.  Default constructor is implicitly declared (i.e. aggregate
/// initialization, which means value is not initialized).  data_[0] is the last significant
/// portion.
template <typename Underlying, size_type N>
struct basic_fixed_int {
   public:
    using underlying_type = Underlying;
    constexpr static size_type size = N;

   private:
    std::array<underlying_type, size> data_;

   public:
    /// Defaulted default constructor.  Defaulting the default constructor ensures that
    /// `basic_fixed_int x` will be uninitialized but `basic_fixed_int y{}` will be zeroed.
    constexpr basic_fixed_int() noexcept = default;

    /// Constructs and returns a value that represents val.  This does nothing (i.e. it is
    /// well-defined) if N == 0.
    constexpr basic_fixed_int(underlying_type val) noexcept
        : data_{detail::make_array_from_repeated_value_with_different_first<underlying_type, size>(
              0,
              val)} {}

    /// Constructs and returns a value that represents val.  This does nothing (i.e. it is
    /// well-defined) if N == 0.
    constexpr basic_fixed_int(const std::array<underlying_type, size>& other) noexcept
        : data_{other} {}

   private:
    template <size_type I>
    constexpr unsigned char inc_impl(unsigned char carry) noexcept {
        if constexpr (I < N) {
            carry = detail::intrin::addcarry(carry, data_[I], 0, data_[I]);
            return inc_impl<I + 1>(carry);
        } else {
            return carry;
        }
    }

    template <size_type I, size_t OtherN>
    constexpr unsigned char add_impl(unsigned char carry, const underlying_type* other) noexcept {
        if constexpr (I < N && I < OtherN) {
            carry = detail::intrin::addcarry(carry, data_[I], other[I], data_[I]);
            return add_impl<I + 1, OtherN>(carry, other);
        } else {
            return carry;
        }
    }

   public:
    // Lots of operators.  When N==1, we don't need add-with-carry, and that is usually
    // special-cased.  This perhaps helps the compiler avoid the slow 8/16 bit add-with-carry shim
    // for small integers.

    constexpr basic_fixed_int& operator+=(underlying_type val) noexcept {
        if constexpr (N > 1) {
            unsigned char carry = detail::intrin::addcarry(0, data_[0], val, data_[0]);
            inc_impl<1>(carry);
        } else if constexpr (N == 1) {
            data_[0] += val;
        } else {
            // N==0, do nothing
        }
        return *this;
    }
    constexpr basic_fixed_int operator+(underlying_type val) const noexcept {
        basic_fixed_int res = *this;
        res += val;
        return res;
    };

    template <typename U2, size_type N2>
    constexpr basic_fixed_int& operator+=(const basic_fixed_int<U2, N2>& val) noexcept {
        if constexpr (std::is_same_v<underlying_type, U2>) {
            // If both underlying types are same, we just add until the minimum length, then do
            // increments (second addend would be zero in that position)
            if constexpr (std::min(N, N2) > 0) {
                if constexpr (N == 1) {
                    data_[0] += val;
                } else {
                    unsigned char carry = add_impl<0, N2>(0, val.data());
                    if (N > N2) {
                        // we need to do increments
                        inc_impl<N2>(carry);
                    }
                }
            } else {
                // std::min(N, N2)==0, do nothing
            }

        } else {
            // TODO
            static_assert(detail::always_false_v<U2>,
                          "Operations with different underlying type is not implemented yet.");
        }
        return *this;
    }
    // TODO this is not right, it should use the wider of the two ints.
    template <typename U2, size_type N2>
    constexpr underlying_type operator+(const basic_fixed_int<U2, N2>& val) const noexcept {
        underlying_type res = *this;
        res += val;
        return res;
    }

    underlying_type* data() noexcept { return data_.data(); }
    const underlying_type* data() const noexcept { return data_.data(); }
};

/// Alias for basic_fixed_int with at least `Bits` bits (base 2).
/// Will use less than twice as much space as necessary.
template <size_type Bits>
using fixed_int =
    basic_fixed_int<detail::underlying_int_t<detail::best_int_v<Bits>>,
                    (Bits + (detail::best_int_v<Bits> - 1)) / detail::best_int_v<Bits>>;
}  // namespace big
