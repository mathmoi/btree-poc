#include <btree/NodeBase.hpp>

#include "catch2/catch_test_macros.hpp"

#include <cstdint>

namespace btreepoc::btree::tests
{
    TEST_CASE("NodeBase::NodeBase A newly constructed nodes is empty")
    {
        typedef NodeBase<std::uint32_t, std::uint32_t> NodeType;

        const NodeType::SizeType expected = 0;

        NodeType node(3);
        auto actual = node.size();

        REQUIRE(expected == actual);
    }

    TEST_CASE("NodeBase::Insert After inserting a cell into a new node, the size is 1")
    {
        typedef NodeBase<std::uint32_t, std::uint32_t> NodeType;

        const NodeType::SizeType expected = 1;
        const NodeType::CellType cell     = { 1, 2 };

        NodeType node(3);
        node.Insert(cell);
        auto actual = node.size();

        REQUIRE(expected == actual);
    }

    TEST_CASE(
      "NodeBase::operator[] After a cell is inserted it can be read back with the indexing operator")
    {
        typedef NodeBase<std::uint32_t, std::uint32_t> NodeType;

        const NodeType::CellType cell = { 1, 2 };

        NodeType node(3);
        node.Insert(cell);
        auto result = node[0];

        REQUIRE(cell.key == result.key);
        REQUIRE(cell.value == result.value);
    }

    TEST_CASE(
      "NodeBase::Insert When two cell are inserted, the lowest key is in position zero regardless of the order in which they are inserted")
    {
        typedef NodeBase<std::uint32_t, std::uint32_t> NodeType;

        const NodeType::CellType lowest_key_cell  = { 1, 2 };
        const NodeType::CellType highest_key_cell = { 3, 6 };

        NodeType node(3);

        SECTION("lowest key inserted first")
        {
            node.Insert(lowest_key_cell);
            node.Insert(highest_key_cell);
        }

        SECTION("highest key inserted first")
        {
            node.Insert(highest_key_cell);
            node.Insert(lowest_key_cell);
        }

        REQUIRE(lowest_key_cell.key == node[0].key);
        REQUIRE(highest_key_cell.key == node[1].key);
    }

    TEST_CASE(
      "NodeBase::Insert When inserting a cell with a key already present in the node, the insertion should fail")
    {
        typedef NodeBase<std::uint32_t, std::uint32_t> NodeType;

        const NodeType::CellType first_cell  = { 1, 2 };
        const NodeType::CellType second_cell = { 1, 6 };

        NodeType node(3);
        auto first_insertion_result  = node.Insert(first_cell);
        auto second_insertion_result = node.Insert(second_cell);

        REQUIRE(first_insertion_result);
        REQUIRE_FALSE(second_insertion_result);
    }

    TEST_CASE(
      "NodeBase::underflow must return true when the node contains less than `ceil(order / 2) - 1` nodes")
    {
        typedef NodeBase<std::uint32_t, std::uint32_t> NodeType;

        const NodeType::SizeType order = 5;

        NodeType node(order);

        // size = 0
        REQUIRE(node.underflow());

        // size = 1
        node.Insert({ 1, 2 });
        REQUIRE(node.underflow());

        // size = 2
        node.Insert({ 2, 2 });
        REQUIRE_FALSE(node.underflow());
    }

    TEST_CASE(
      "NodeBase::overflow must return true when the node contains more than `order - 1` nodes")
    {
        typedef NodeBase<std::uint32_t, std::uint32_t> NodeType;

        const NodeType::SizeType order = 5;

        NodeType node(order);

        // size = 5
        REQUIRE_FALSE(node.overflow());

        // size = 1..4
        for(std::uint32_t x = 0; x < 4; ++x)
        {
            node.Insert({ x, 2 });
            REQUIRE_FALSE(node.overflow());
        }

        // size = 5
        node.Insert({ 5, 2 });
        REQUIRE(node.overflow());
    }

} // namespace btreepoc::btree::tests