#include <iostream>

#include "asio.hpp"


int main()
{
    asio::io_context io_context;
    io_context.run();

    std::cout << "We will see this line in console windows.\n";

    return 0;
}
