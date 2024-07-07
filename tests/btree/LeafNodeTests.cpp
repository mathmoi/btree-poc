#include <btree/LeafNode.hpp>

#include "catch2/catch_test_macros.hpp"

namespace btreepoc::btree::tests
{
    TEST_CASE("LeafNode::is_leaf Always returns true")
    {
        typedef LeafNode<std::uint32_t, std::uint32_t> NodeType;

        const NodeType::Base::SizeType order = 3;

        NodeType node(order);
        bool is_leaf = node.is_leaf();

        REQUIRE(is_leaf);
    }
} // namespace btreepoc::btree::tests