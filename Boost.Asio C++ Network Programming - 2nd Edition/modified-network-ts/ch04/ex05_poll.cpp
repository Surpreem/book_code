#include <iostream>

#include "asio.hpp"


int main()
{
    asio::io_context io_context;

    for (auto i{0}; 5 != i; ++i) {
        io_context.poll();
        std::cout << "Line: " << i << '\n';
    }

    return 0;
}
