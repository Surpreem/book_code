#include <functional>
#include <iostream>
#include <string>


class TheClass {
public:
    void identity(std::string name, int age, float height)
    {
        std::cout << "Name   : " << name << '\n';
        std::cout << "Age    : " << age << " years old\n";
        std::cout << "Height : " << height << " inch\n";
    }
};


int main()
{
    TheClass cls;
    std::bind(&TheClass::identity, &cls, "John", 25, 68.98f)();

    return 0;
}
