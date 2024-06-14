#include "adder/Adder.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Adder::Add two plus two returns four")
{
    project::adder::Adder sut;

    auto result = sut.Add(2, 2);

    REQUIRE(4 == result);
}