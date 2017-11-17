#include <functional>
#include <iostream>
#include <string>


void identity(std::string name, int age, float height)
{
    std::cout << "Name   : " << name << '\n';
    std::cout << "Age    : " << age << " years old" << '\n';
    std::cout << "Height : " << height << " inch" << '\n';
}

int main()
{
    std::bind(identity, "John", 25, 68.89f)();

    return 0;
}
