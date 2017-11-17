#include <iostream>

#include "asio.hpp"


int main()
{
    asio::io_service io_svc;
    asio::io_service::work work(io_svc);

    for (auto i{0}; 5 != i; ++i) {
        io_svc.poll();
        std::cout << "Line: " << i << '\n';
    }

    return 0;
}
