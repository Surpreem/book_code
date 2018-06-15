#include <iostream>

#include "asio.hpp"


int main()
{
    asio::io_context io_context;
//    asio::executor_work_guard<asio::io_context::executor_type>
//        worker{asio::make_work_guard(io_context)};
    asio::executor_work_guard worker{asio::make_work_guard(io_context)};

    io_context.run();

    std::cout << "We will not see this line in console window. :(\n";

    return 0;
}
