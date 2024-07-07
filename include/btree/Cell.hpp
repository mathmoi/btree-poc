#ifndef BTREE_POC_BTREE_CELL_HPP_
#define BTREE_POC_BTREE_CELL_HPP_

namespace btreepoc::btree
{
    // The cell class contains a key-value pair that can be contained in a node of a
    // btree. we could have used a std::pair instead, but since the our eventual
    // implementation of a btree will need this class to handle how the key/value pairs
    // will be stored on disk it's a good idea to have this class in the POC also.
    template<typename TKey, typename TValue>
    struct Cell
    {
        // Key
        TKey key;

        // Value
        TValue value;
    };
} // namespace btreepoc::btree

#endif // BTREE_POC_BTREE_CELL_HPP_