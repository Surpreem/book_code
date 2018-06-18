#include <functional>
#include <iostream>


void cubevolume(float f)
{
    std::cout << "Volume of the cube is " << f * f * f << '\n';
}


int main()
{
    std::bind(cubevolume, 4.23f)();

    return 0;
}
