#include "adder/Adder.hpp"

#include <cstdlib>
#include <iostream>

int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <int1> <int2>" << std::endl;
        return 1;
    }

    int num1 = std::atoi(argv[1]);
    int num2 = std::atoi(argv[2]);

    project::adder::Adder adder;
    int sum = adder.Add(num1, num2);

    std::cout << "The sum is: " << sum << std::endl;

    return 0;
}