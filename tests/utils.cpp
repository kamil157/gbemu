#include <utils.h>

#include <catch.hpp>

TEST_CASE("Bits are numbered from right to left, from 0 to 7", "[utils]")
{
    REQUIRE(isBitSet(0b00000001, 0));
    REQUIRE(isBitSet(0b10000000, 7));
    REQUIRE(!isBitSet(0b10000000, 0));
}

TEST_CASE("Bytes are concatenated as little endian", "[utils]")
{
    REQUIRE(concatBytes(0x34, 0x12) == 0x1234);
}

TEST_CASE("Half carry occurs when, while adding two bytes, bit 3 carries over to bit 4", "[utils]")
{
    REQUIRE(isHalfCarryAddition(0xf, 0x1)); // 0xf + 0x1 = 0x10 > 0xf
    REQUIRE(!isHalfCarryAddition(0x8, 0x7)); // 0x8 + 0x7 = 0xf <= 0xf
    REQUIRE(!isHalfCarryAddition(0xff, 0xf0)); // 0xf + 0x0 = 0xf <= 0xf
}

TEST_CASE("Half carry occurs when, while subtracting two bytes, bit 4 does not borrow from bit 3", "[utils]")
{
    REQUIRE(isHalfCarrySubtraction(0x10, 0x1)); // 0x0 - 0x1 = -0x1 < 0
    REQUIRE(isHalfCarrySubtraction(0x10, -0x1)); // 0x0 - 0xf = -0xf < 0
    REQUIRE(!isHalfCarrySubtraction(0xf, -0x7)); // 0xf - 0x9 = 0x7 >= 0
    REQUIRE(!isHalfCarrySubtraction(static_cast<int8_t>(0xff), static_cast<int8_t>(0xf0))); // 0xf - 0x0 = 0xf >= 0
}

TEST_CASE("Half carry occurs when, while adding two 16 bit values, bit 11 carries over to bit 12", "[utils]")
{
    REQUIRE(isHalfCarryAddition16(0xfff, 0x1)); // 0xfff + 0x1 = 0x1000 > 0xfff
    REQUIRE(!isHalfCarryAddition16(0x800, 0x7ff)); // 0x800 + 0x7ff = 0xfff <= 0xfff
    REQUIRE(!isHalfCarryAddition16(0xffff, 0xf000)); // 0xfff + 0xf000 = 0xfff <= 0xfff
}
