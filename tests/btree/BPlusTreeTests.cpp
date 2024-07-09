#include <btree/BPlusTree.hpp>

#include "builders/BPlusTreeBuilder.hpp"

#include <catch2/catch_test_macros.hpp>

namespace btreepoc::btree::tests
{
    typedef std::uint32_t KeyType;
    typedef std::uint32_t ValueType;
    typedef BPlusTree<KeyType, ValueType> BTreeType;
    typedef builders::BPlusTreeBuilder<KeyType, ValueType> BuilderType;

    TEST_CASE(
      "BTree::Insert When we insert a key/value pair the insertion returns a succes")
    {
        const BTreeType::SizeType order = 4;
        const KeyType key               = 1;
        const ValueType value           = 2;

        BTreeType btree(order);
        auto result = btree.Insert(key, value);

        REQUIRE(result);
    }

    TEST_CASE(
      "BTree::Insert When we insert the same key twice, the second insertion fails")
    {
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
      "BTree::Insert When we insert multiple key/value pairs with differents keys all insertion succeed")
    {
        const BTreeType::SizeType order = 4;
        const ValueType value           = 2;

        BTreeType btree(order);

        for(KeyType key = 0; key < order - 1; ++key)
        {
            auto result = btree.Insert(key, value);
            REQUIRE(result);
        }
    }

    //           Insert 3
    //              │
    //              ▼
    //         ┌────┬────┐
    //         │  1 │  2 │
    //         └────┴────┘
    //
    //            Result
    //              │
    //              ▼
    //         ┌────┬────┐
    //      ┌──┤  2 │    ├──┐
    //      │  └────┴────┘  │
    //      │               │
    //      ▼               ▼
    // ┌────┬────┐     ┌────┬────┐
    // │  1 │  2 │     │  3 │    │
    // └────┴────┘     └────┴────┘
    TEST_CASE(
      "BTree::Insert when the root node is full and we add another key/value pair the root should split.")
    {
        BuilderType builder; // clang-format off
        auto btree = builder.WithOrder(3)
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

    //              Insert 35
    //                  │
    //                  ▼
    //             ┌────┬────┐
    //      ┌──────┤ 20 │ 40 ├──────┐
    //      │      └────┼────┘      │
    //      │           │           │
    //      ▼           ▼           ▼
    // ┌────┬────┐ ┌────┬────┐ ┌────┬────┐
    // │ 10 │ 20 │ │ 30 │ 40 │ │ 50 │    │
    // └────┴────┘ └────┴────┘ └────┴────┘
    //
    //
    //                Result
    //                  │
    //                  ▼
    //             ┌────┬────┐
    //      ┌──────┤ 20 │ 35 ├──────┐
    //      │      └────┼────┘      │
    //      │           │           │
    //      ▼           ▼           ▼
    // ┌────┬────┐ ┌────┬────┐ ┌────┬────┐
    // │ 10 │ 20 │ │ 30 │ 35 │ │ 40 │ 50 │
    // └────┴────┘ └────┴────┘ └────┴────┘
    TEST_CASE(
      "BPlusTree::Insert When a LeafNodes overflow, it can send a cell to it's right sibling if space is available")
    {
        BuilderType builder; // clang-format off
        auto btree = builder.WithOrder(3)
                            .AddInternalNode()
                                .AddLeafNode(20)
                                    .AddKeys({10, 20})
                                .EndNode()
                                .AddLeafNode(40)
                                    .AddKeys({30, 40})
                                .EndNode()
                                .AddLeafNode()
                                    .AddKeys({50})
                                .EndNode()
                            .EndNode()
                            .Build();

        auto expected = builder.WithOrder(3)
                                .AddInternalNode()
                                    .AddLeafNode(20)
                                        .AddKeys({10, 20})
                                    .EndNode()
                                    .AddLeafNode(35)
                                        .AddKeys({30, 35})
                                    .EndNode()
                                    .AddLeafNode()
                                        .AddKeys({40, 50})
                                    .EndNode()
                                .EndNode()
                                .Build(); // clang-format on

        btree.Insert(35, 70);

        REQUIRE(expected.CompareKeys(btree));
    }

    //                             Insert 25
    //                                 │
    //                                 ▼
    //                          ┌────┬────┬────┐
    //         ┌────────────────┤ 30 │ 60 │ 70 ├──────────────────┐
    //         │                └──┬─┴────┴──┬─┘                  │
    //         │                   │         │                    │
    //         ▼                   ▼         ▼                    ▼
    // ┌────┬────┬────┐ ┌────┬────┬────┐ ┌────┬────┬────┐ ┌────┬────┬────┐
    // │ 10 │ 20 │ 30 │ │ 40 │ 50 │ 60 │ │ 70 │    │    │ │ 80 │    │    │
    // └────┴────┴────┘ └────┴────┴────┘ └────┴────┴────┘ └────┴────┴────┘
    //
    //
    //                               Result
    //                                 │
    //                                 ▼
    //                          ┌────┬────┬────┐
    //         ┌────────────────┤ 25 │ 50 │ 70 ├──────────────────┐
    //         │                └──┬─┴────┴──┬─┘                  │
    //         │                   │         │                    │
    //         ▼                   ▼         ▼                    ▼
    // ┌────┬────┬────┐ ┌────┬────┬────┐ ┌────┬────┬────┐ ┌────┬────┬────┐
    // │ 10 │ 20 │ 25 │ │ 30 │ 40 │ 50 │ │ 60 │ 70 │    │ │ 80 │    │    │
    // └────┴────┴────┘ └────┴────┴────┘ └────┴────┴────┘ └────┴────┴────┘
    TEST_CASE(
      "BPlusTree::Insert When the overflowing node is the first node of four childre, the first tree nodes are balanced")
    {
        BuilderType builder; // clang-format off
        auto btree = builder.WithOrder(4)
                            .AddInternalNode()
                                .AddLeafNode(30)
                                    .AddKeys({10, 20, 30})
                                .EndNode()
                                .AddLeafNode(60)
                                    .AddKeys({40, 50, 60})
                                .EndNode()
                                .AddLeafNode(70)
                                    .AddKeys({70})
                                .EndNode()
                                .AddLeafNode()
                                    .AddKeys({80})
                                .EndNode()
                            .EndNode()
                            .Build();

        auto expected = builder.WithOrder(4)
                                .AddInternalNode()
                                    .AddLeafNode(25)
                                        .AddKeys({10, 20, 25})
                                    .EndNode()
                                    .AddLeafNode(50)
                                        .AddKeys({30, 40, 50})
                                    .EndNode()
                                    .AddLeafNode(70)
                                        .AddKeys({60, 70})
                                    .EndNode()
                                    .AddLeafNode()
                                        .AddKeys({80})
                                    .EndNode()
                                .EndNode()
                                .Build(); // clang-format on

        btree.Insert(25, 50);

        REQUIRE(expected.CompareKeys(btree));
    }

    //                             Insert 75
    //                                 │
    //                                 ▼
    //                          ┌────┬────┬────┐
    //         ┌────────────────┤ 10 │ 30 │ 50 ├──────────────────┐
    //         │                └──┬─┴────┴──┬─┘                  │
    //         │                   │         │                    │
    //         ▼                   ▼         ▼                    ▼
    // ┌────┬────┬────┐ ┌────┬────┬────┐ ┌────┬────┬────┐ ┌────┬────┬────┐
    // │ 10 │    │    │ │ 20 │ 30 │    │ │ 40 │ 50 │    │ │ 60 │ 70 │ 80 │
    // └────┴────┴────┘ └────┴────┴────┘ └────┴────┴────┘ └────┴────┴────┘
    //
    //
    //                               Result
    //                                 │
    //                                 ▼
    //                          ┌────┬────┬────┐
    //         ┌────────────────┤ 10 │ 40 │ 70 ├──────────────────┐
    //         │                └──┬─┴────┴──┬─┘                  │
    //         │                   │         │                    │
    //         ▼                   ▼         ▼                    ▼
    // ┌────┬────┬────┐ ┌────┬────┬────┐ ┌────┬────┬────┐ ┌────┬────┬────┐
    // │ 10 │    │    │ │ 20 │ 30 │ 40 │ │ 50 │ 60 │ 70 │ │ 75 │ 80 │    │
    // └────┴────┴────┘ └────┴────┴────┘ └────┴────┴────┘ └────┴────┴────┘
    TEST_CASE(
      "BPlusTree::Insert When the overflowing node is the last node of four childre, the last tree nodes are balanced")
    {
        BuilderType builder; // clang-format off
        auto btree = builder.WithOrder(4)
                            .AddInternalNode()
                                .AddLeafNode(10)
                                    .AddKeys({10})
                                .EndNode()
                                .AddLeafNode(30)
                                    .AddKeys({20, 30})
                                .EndNode()
                                .AddLeafNode(50)
                                    .AddKeys({40, 50})
                                .EndNode()
                                .AddLeafNode()
                                    .AddKeys({60, 70, 80})
                                .EndNode()
                            .EndNode()
                            .Build();

        auto expected = builder.WithOrder(4)
                                .AddInternalNode()
                                    .AddLeafNode(10)
                                        .AddKeys({10})
                                    .EndNode()
                                    .AddLeafNode(40)
                                        .AddKeys({20, 30, 40})
                                    .EndNode()
                                    .AddLeafNode(70)
                                        .AddKeys({50, 60, 70})
                                    .EndNode()
                                    .AddLeafNode()
                                        .AddKeys({75, 80})
                                    .EndNode()
                                .EndNode()
                                .Build(); // clang-format on

        btree.Insert(75, 150);

        REQUIRE(expected.CompareKeys(btree));
    }

    //                    Insert 70
    //                        │
    //                        ▼
    //                 ┌────┬────┬────┐
    //                 │ 30 │    │    │
    //                 └──┬─┴────┴──┬─┘
    //                    │         │
    //                    ▼         ▼
    //         ┌────┬────┬────┐ ┌────┬────┬────┐
    //         │ 10 │ 20 │ 30 │ │ 40 │ 50 │ 60 │
    //         └────┴────┴────┘ └────┴────┴────┘
    //
    //
    //                     Insert 70
    //                         │
    //                         ▼
    //                  ┌────┬────┬────┐
    //        ┌─────────┤ 30 │ 50 │    ├────────┐
    //        │         └────┴─┬──┴────┘        │
    //        │                │                │
    //        ▼                ▼                ▼
    // ┌────┬────┬────┐ ┌────┬────┬────┐ ┌────┬────┬────┐
    // │ 10 │ 20 │ 30 │ │ 40 │ 50 │    │ │ 60 │ 70 │    │
    // └────┴────┴────┘ └────┴────┴────┘ └────┴────┴────┘
    TEST_CASE(
      "BPlusTree::Insert Leaf node should split when keys can't be moved around siblings")
    {
        BuilderType builder; // clang-format off
        auto btree = builder.WithOrder(4)
                            .AddInternalNode()
                                .AddLeafNode(30)
                                    .AddKeys({10, 20, 30})
                                .EndNode()
                                .AddLeafNode()
                                    .AddKeys({40, 50, 60})
                                .EndNode()
                            .EndNode()
                            .Build();

        auto expected = builder.WithOrder(4)
                                .AddInternalNode()
                                    .AddLeafNode(30)
                                        .AddKeys({10, 20, 30})
                                    .EndNode()
                                    .AddLeafNode(50)
                                        .AddKeys({40, 50})
                                    .EndNode()
                                    .AddLeafNode()
                                        .AddKeys({60, 70})
                                    .EndNode()
                                .EndNode()
                                .Build(); // clang-format on

        btree.Insert(70, 140);

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