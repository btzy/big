#include <catch2/catch_all.hpp>

#include <big.hpp>

TEST_CASE("best_int works", "[best_int]") {
    REQUIRE(big::detail::best_int_v<0> == 8);
    REQUIRE(big::detail::best_int_v<1> == 8);
    REQUIRE(big::detail::best_int_v<7> == 8);
    REQUIRE(big::detail::best_int_v<8> == 8);
    REQUIRE(big::detail::best_int_v<9> == 16);
    REQUIRE(big::detail::best_int_v<16> == 16);
    REQUIRE(big::detail::best_int_v<17> == 32);
    REQUIRE(big::detail::best_int_v<64> == 64);
    REQUIRE(big::detail::best_int_v<65> == 64);
    REQUIRE(big::detail::best_int_v<1000> == 64);
}
