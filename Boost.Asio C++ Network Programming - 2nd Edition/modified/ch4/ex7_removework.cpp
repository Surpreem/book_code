#include <iostream>
#include <memory>

#include "asio.hpp"


int main()
{
    asio::io_service io_svc;
    auto worker{std::make_shared<asio::io_service::work>(io_svc)};
    worker.reset();

    io_svc.run();

    std::cout << "We will not see this line in console window :(\n";

    return 0;
}
