#include <iostream>


void print1()
{
    for (auto i{0}; 5 != i; ++i)
        std::cout << "[print1] Line: " << i << '\n';
}

void print2()
{
    for (auto i{0}; 5 != i; ++i)
        std::cout << "[print2] Line: " << i << '\n';
}


int main()
{
    print1();
    print2();

    return 0;
}
