#include <iostream>

#include "asio.hpp"


int main()
{
    asio::io_service io_svc;
    asio::io_service::work worker(io_svc);

    io_svc.run();

    std::cout << "We will not see this line in console window : (\n";

    return 0;
}
