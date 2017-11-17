#include <iostream>

#include "asio.hpp"


int main()
{
    asio::io_service io_svc;
    io_svc.run();

    std::cout << "We will see this line in console window.\n";

    return 0;
}
