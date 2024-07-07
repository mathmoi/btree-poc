#include <btree/BPlusTree.hpp>

#include "builders/BPlusTreeBuilder.hpp"

#include <catch2/catch_test_macros.hpp>

namespace btreepoc::btree::tests
{
    TEST_CASE(
      "BTree::Insert When we insert a key/value pair the insertion returns a succes")
    {
        typedef std::uint32_t KeyType;
        typedef std::uint32_t ValueType;
        typedef BPlusTree<KeyType, ValueType> BTreeType;

        const BTreeType::OrderType order = 4;
        const KeyType key                = 1;
        const ValueType value            = 2;

        BTreeType btree(order);
        auto result = btree.Insert(key, value);

        REQUIRE(result);
    }

    TEST_CASE(
      "BTree::Insert When we insert the same key twice, the second insertion fails")
    {
        typedef std::uint32_t KeyType;
        typedef std::uint32_t ValueType;
        typedef builders::BPlusTreeBuilder<KeyType, ValueType> BuilderType;

        const KeyType key     = 1;
        const ValueType value = 2;

        BuilderType builder; // clang-format off
        auto btree = builder.AddLeafNode()
                                .AddKeys({ key })
                            .EndNode()
                            .Build(); // clang-format on

        auto result = btree.Insert(key, value);

        REQUIRE_FALSE(result);
    }

    TEST_CASE(
      "BTreeInsert When we insert multiple key/value pairs with differents keys all insertion succeed")
    {
        typedef std::uint32_t KeyType;
        typedef std::uint32_t ValueType;
        typedef BPlusTree<KeyType, ValueType> BTreeType;

        const BTreeType::OrderType order = 4;
        const ValueType value            = 2;

        BTreeType btree(order);

        for(KeyType key = 0; key < order - 1; ++key)
        {
            auto result = btree.Insert(key, value);
            REQUIRE(result);
        }
    }

    //             Insert 3
    //                │
    //                ▼
    //           ┌─────────┐
    //           │   1, 2  │
    //           └─────────┘
    //
    //             Result
    //                │
    //                ▼
    //           ┌─────────┐
    //      ┌────┤    2    ├────┐
    //      │    └─────────┘    │
    //      ▼                   ▼
    // ┌─────────┐         ┌─────────┐
    // │   1, 2  │         │    3    │
    // └─────────┘         └─────────┘
    TEST_CASE(
      "BTree::Insert when the root node is full and we add another key/value pair the root should split.")
    {
        typedef std::uint32_t KeyType;
        typedef std::uint32_t ValueType;
        typedef builders::BPlusTreeBuilder<KeyType, ValueType> BuilderType;

        BuilderType builder; // clang-format off
        auto btree = builder.WithOrder(2)
                            .AddLeafNode()
                                .AddKeys({ 1, 2 })
                            .EndNode()
                            .Build();

        auto expected = builder.WithOrder(2)
                               .AddInternalNode()
                                    .AddLeafNode(2)
                                        .AddKeys({1, 2})
                                    .EndNode()
                                    .AddLeafNode()
                                        .AddKeys({ 3 })
                                    .EndNode()
                               .EndNode()
                               .Build(); // clang-format on

        btree.Insert(3, 6);

        REQUIRE(expected.CompareKeys(btree));
    }

    // TEST_CASE("BTree::Insert When we insert a key/value pair, we can read it back.")
    // {
    //     typedef std::uint32_t KeyType;
    //     typedef std::uint32_t ValueType;
    //     typedef BPlusTree<KeyType, ValueType> BTreeType;

    //     const BTreeType::OrderType order = 4;
    //     const KeyType key                = 1;
    //     const ValueType value            = 2;

    //     BTreeType btree(order);
    //     btree.Insert(key, value);
    // }
} // namespace btreepoc::btree::tests