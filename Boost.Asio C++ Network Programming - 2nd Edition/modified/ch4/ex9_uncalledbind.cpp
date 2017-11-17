#include <functional>
#include <iostream>


void func()
{
    std::cout << "Binding Function\n";
}

int main()
{
    std::bind(func);

    return 0;
}
