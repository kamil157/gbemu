#include <cpu.h>
#include <mmu.h>

#include <memory>

#include <catch.hpp>

TEST_CASE("Cpu starts with pc 0", "[cpu]")
{
    byteCodePtr code;
    auto mmu = std::make_unique<Mmu>();
    Cpu cpu{ code, std::move(mmu) };
    REQUIRE(cpu.getPc() == 0);
}
