#ifndef BTREE_B_PLUS_TREE_HPP_
#define BTREE_B_PLUS_TREE_HPP_

#include "InternalNode.hpp"
#include "LeafNode.hpp"

#include <optional>

namespace btree
{
    template<typename TKey, typename TValue, const int order>
    class BPlusTree
    {
    public:
        typedef LeafNode<TKey, TValue, order> LeafNodeType;
        typedef InternalNode<TKey, order> InternalNodeType;

        BPlusTree()
        : root_(
            std::make_shared<LeafNode<TKey, TValue, order>>()) // We create the btree with
                                                               // an empty node because
                                                               // that's the way it's
                                                               // gonna be in mkvdb
        {
        }

        void Insert(const TKey& key, const TValue& value)
        {
            auto new_node = InsertInternal(root_, key, value);
            if(new_node)
            {
                auto new_root = std::make_shared<InternalNode<TKey, order>>();
                new_root->Insert(root_, new_node->first, new_node->second);
                root_ = new_root;
            }
        }

        void Print(std::ostream& out) const
        {
            out << "digraph BTree {\n";
            out << "node [shape=record];\n";
            PrintInternal(root_, out);
            out << "}\n";
        }

    private:
        typename InternalNodeType::ChildPointer root_;

        std::optional<std::pair<TKey, typename InternalNodeType::ChildPointer>>
          InsertInternal(typename InternalNodeType::ChildPointer node,
                         const TKey& key,
                         const TValue& value)
        {
            if(node->is_leaf())
            {
                auto leaf = std::static_pointer_cast<LeafNode<TKey, TValue, order>>(node);
                if(leaf->has_space())
                {
                    leaf->Insert(key, value);
                    return std::nullopt;
                }
                else
                {
                    auto new_node = leaf->Split();
                    (key < new_node.first ? leaf : new_node.second)->Insert(key, value);
                    return new_node;
                }
            }
            else
            {
                auto internal = std::static_pointer_cast<InternalNode<TKey, order>>(node);
                auto child_node      = internal->FindChild(key);
                auto maybe_new_child = InsertInternal(child_node, key, value);
                if(maybe_new_child)
                {
                    if(internal->has_space())
                    {
                        internal->Insert(maybe_new_child->first, maybe_new_child->second);
                    }
                    else
                    {
                        // If the current node has not space we need to split, then
                        // insert, then return a new node again.
                        auto new_node = internal->Split();
                        (maybe_new_child->first < new_node.first ? internal
                                                                 : new_node.second)
                          ->Insert(maybe_new_child->first, maybe_new_child->second);
                        return new_node;
                    }
                }
            }

            return std::nullopt;
        }

        void PrintInternal(typename InternalNodeType::ChildPointer node,
                           std::ostream& out) const
        {
            node->Print(out);
            if(InternalNodeType* internal = dynamic_cast<InternalNodeType*>(node.get()))
            {
                for(std::uint32_t x = 0; x < internal->items_count() + 1; ++x)
                {
                    PrintInternal(internal->GetChildAt(x), out);
                }
            }
        }
    };
} // namespace btree

#endif // BTREE_B_PLUS_TREE_HPP_