#include <catch2/catch_all.hpp>

#include <big/big.hpp>

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

TEST_CASE("fixed_int construction from underlying type", "[fixed_int]") {
    using U = uint32_t;
    {
        big::basic_fixed_int<U, 4> b(100);
        REQUIRE(std::vector<U>(b.data(), b.data() + 4) == std::vector<U>{100, 0, 0, 0});
    }
    {
        big::basic_fixed_int<U, 1> b(100);
        REQUIRE(std::vector<U>(b.data(), b.data() + 1) == std::vector<U>{100});
    }
    {
        big::basic_fixed_int<U, 4> b(std::array<U, 4>{100, 200, 300, 451});
        REQUIRE(std::vector<U>(b.data(), b.data() + 4) == std::vector<U>{100, 200, 300, 451});
    }
}

TEST_CASE("fixed_int operator+=", "[fixed_int]") {
    using U = std::uint32_t;
    constexpr U half = std::numeric_limits<U>::max() / 2 + 1;
    {
        big::basic_fixed_int<U, 4> b(100);
        b += 150;
        REQUIRE(std::vector<U>(b.data(), b.data() + 4) == std::vector<U>{250, 0, 0, 0});
    }
    {
        big::basic_fixed_int<U, 1> b(100);
        b += 150;
        REQUIRE(std::vector<U>(b.data(), b.data() + 1) == std::vector<U>{250});
    }
    {
        // Add underlying_type
        big::basic_fixed_int<U, 4> b(std::array<U, 4>{100, 200, 300, 451});
        b += 150;
        REQUIRE(std::vector<U>(b.data(), b.data() + 4) == std::vector<U>{250, 200, 300, 451});
        b += half;
        REQUIRE(std::vector<U>(b.data(), b.data() + 4) ==
                std::vector<U>{half + 250, 200, 300, 451});
        b += half; // overflows the first digit
        REQUIRE(std::vector<U>(b.data(), b.data() + 4) == std::vector<U>{250, 201, 300, 451});
    }
    {
        // Add another big_int (smaller)
        big::basic_fixed_int<U, 4> b(std::array<U, 4>{100, 200, 300, 451});
        b += big::basic_fixed_int<U, 2>(std::array<U, 2>{123, 456});
        REQUIRE(std::vector<U>(b.data(), b.data() + 4) == std::vector<U>{223, 656, 300, 451});
    }
    {
        // Add another big_int (equal size)
        big::basic_fixed_int<U, 4> b(std::array<U, 4>{100, 200, 300, 451});
        b += big::basic_fixed_int<U, 4>(std::array<U, 4>{123, 456, 789, 120});
        REQUIRE(std::vector<U>(b.data(), b.data() + 4) == std::vector<U>{223, 656, 1089, 571});
    }
    {
        // Add another big_int (larger size)
        big::basic_fixed_int<U, 4> b(std::array<U, 4>{100, 200, 300, 451});
        b += big::basic_fixed_int<U, 5>(std::array<U, 5>{123, 456, 789, 120, 0});
        REQUIRE(std::vector<U>(b.data(), b.data() + 4) == std::vector<U>{223, 656, 1089, 571});
        b += big::basic_fixed_int<U, 5>(
            std::array<U, 5>{0, 0, 0, 0, 1});  // overflows (check if ignored)
        REQUIRE(std::vector<U>(b.data(), b.data() + 4) == std::vector<U>{223, 656, 1089, 571});
    }
    {
        // Add another big_int (smaller with overflow)
        big::basic_fixed_int<U, 4> b(std::array<U, 4>{half, half + 1, half + 2, half + 3});
        b += big::basic_fixed_int<U, 2>(std::array<U, 2>{half + 4, half + 5});
        REQUIRE(std::vector<U>(b.data(), b.data() + 4) == std::vector<U>{4, 7, half + 3, half + 3});
    }
    {
        // Add another big_int (equal size)
        big::basic_fixed_int<U, 4> b(std::array<U, 4>{half, half + 1, half + 2, half + 3});
        b += big::basic_fixed_int<U, 4>(std::array<U, 4>{half + 4, half + 5, half + 6, 1});
        REQUIRE(std::vector<U>(b.data(), b.data() + 4) == std::vector<U>{4, 7, 9, half + 5});
        b += big::basic_fixed_int<U, 4>(
            std::array<U, 4>{half + 7, half + 8, half + 9, half + 10});  // overflows
        REQUIRE(std::vector<U>(b.data(), b.data() + 4) ==
                std::vector<U>{half + 11, half + 15, half + 18, 15});
    }
    {
        // Add another big_int (larger size)
        big::basic_fixed_int<U, 4> b(std::array<U, 4>{half, half + 1, half + 2, half + 3});
        b += big::basic_fixed_int<U, 5>(std::array<U, 5>{half + 4, half + 5, half + 6, 1, 0});
        REQUIRE(std::vector<U>(b.data(), b.data() + 4) == std::vector<U>{4, 7, 9, half + 5});
        b += big::basic_fixed_int<U, 5>(std::array<U, 5>{0, 0, 0, 1, 1});  // overflows
        REQUIRE(std::vector<U>(b.data(), b.data() + 4) == std::vector<U>{4, 7, 9, half + 6});
    }
}
