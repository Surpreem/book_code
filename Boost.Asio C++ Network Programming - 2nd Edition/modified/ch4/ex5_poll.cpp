#include <iostream>

#include "asio.hpp"


int main()
{
    asio::io_service io_svc;
    for (auto i{0}; i != 5; ++i) {
        io_svc.poll();
        std::cout << "Line: " << i << '\n';
    }

    return 0;
}
