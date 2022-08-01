#include <array>
#include <cstddef>
#include <type_traits>

namespace big {
namespace detail {

/// Helper.  Don't call from outside this file.
template <typename T, size_t... Is>
constexpr std::array<T, sizeof...(Is) + 1> make_array_from_repeated_value_with_different_first_impl(
    T val,
    T first,
    std::index_sequence<Is...>) noexcept {
    return {first, (static_cast<void>(Is), val)...};
}

/// Returns std::array<T, N>{first, val, ..., val}.
template <typename T, size_t N>
constexpr std::array<T, N> make_array_from_repeated_value_with_different_first(T val,
                                                                               T first) noexcept {
    if constexpr (N > 0) {
        return make_array_from_repeated_value_with_different_first_impl(
            val, first, std::make_index_sequence<N - 1>{});
    } else {
        return {};
    }
}

/// A type trait that is always false, but depends on T so it can be used in a static_assert.
template <typename T>
using always_false = std::negation<std::is_same<T, T>>;
template <typename T>
constexpr inline bool always_false_v = always_false<T>::value;

}  // namespace detail
}  // namespace big
