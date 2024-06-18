
#include "btree/BPlusTree.hpp"

#include <cstdint>
#include <iostream>

int main()
{
    btree::BPlusTree<std::uint32_t, std::uint32_t, 3> tree;

    for(std::uint32_t n = 16; n <= 25; ++n)
    {
        std::cout << "-- " << n << " ------------------------------------\n";

        for(std::uint32_t x = 0; x < n; ++x)
        {
            tree.Insert(x, x * x);
        }

        tree.Print(std::cout);
        std::cout.flush();
    }

    return 0;
}