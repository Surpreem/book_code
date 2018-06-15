#include <iostream>

#include "asio.hpp"


int main()
{
    asio::io_context io_context;
//    asio::executor_work_guard<asio::io_context::executor_type>
//        worker{asio::make_work_guard(io_context)};
    asio::executor_work_guard worker{asio::make_work_guard(io_context)};

    for (auto i{0}; 5 != i; ++i) {
        io_context.poll();  // run all handlers that are ready to run and return
        std::cout << "Line: " << i << '\n';
    }

    return 0;
}
