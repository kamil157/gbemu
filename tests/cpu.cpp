#include <cpu.h>
#include <mmu.h>

#include <memory>

#include <catch.hpp>

TEST_CASE("Cpu starts with pc 0", "[cpu]")
{
    Mmu mmu;
    Cpu cpu{ mmu };
    REQUIRE(cpu.getPC() == 0);
}

TEST_CASE("AF register can be set and retrieved", "[cpu]")
{
    Mmu mmu;
    Cpu cpu{ mmu };

    SECTION("AF is initially 0")
    {
        REQUIRE(cpu.getAF() == 0);
    }
    SECTION("A can be set and retrieved")
    {
        cpu.setAF(0x1200);
        REQUIRE(cpu.getAF() == 0x1200);
    }
    SECTION("F can be set and retrieved")
    {
        cpu.setAF(0x0034);
        REQUIRE(cpu.getAF() == 0x0034);
    }
    SECTION("AF can be set and retrieved")
    {
        cpu.setAF(0x1234);
        REQUIRE(cpu.getAF() == 0x1234);
    }
}
