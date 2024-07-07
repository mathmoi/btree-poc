#include <btree/InternalNode.hpp>

#include "catch2/catch_test_macros.hpp"

#include <memory>

namespace btreepoc::btree::tests
{
    TEST_CASE("InternalNode::is_leaf Always returns false")
    {
        typedef InternalNode<std::uint32_t> NodeType;

        const NodeType::Base::SizeType order = 3;

        NodeType node(order);
        bool is_leaf = node.is_leaf();

        REQUIRE_FALSE(is_leaf);
    }

    TEST_CASE(
      "InternalNode::right_most_pointer By default, the right most pointer is not set.")
    {
        typedef InternalNode<std::uint32_t> NodeType;

        const NodeType::Base::SizeType order = 3;

        NodeType node(order);
        auto result = node.right_most_pointer();

        REQUIRE_FALSE(result);
    }

    TEST_CASE(
      "InternalNode::right_most_pointer After setting the pointer we can read it back")
    {
        typedef InternalNode<std::uint32_t> NodeType;

        const NodeType::Base::SizeType order = 3;

        NodeType node(order);
        auto child = std::make_shared<NodeType>(order);
        node.set_right_most_pointer(child);

        auto result = node.right_most_pointer();

        REQUIRE(child == result);
    }
} // namespace btreepoc::btree::tests