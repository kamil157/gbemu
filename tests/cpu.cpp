#include <cpu.h>
#include <mmu.h>

#include <memory>

#include <catch.hpp>

TEST_CASE("Cpu starts with pc 0", "[cpu]")
{
    Cpu cpu{ std::make_unique<Mmu>() };
    REQUIRE(cpu.getPc() == 0);
}
