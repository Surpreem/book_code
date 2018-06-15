#include <iostream>
#include <memory>

#include "asio.hpp"


int main()
{
    asio::io_context io_context;
//    auto worker{std::make_shared<
//        asio::executor_work_guard<asio::io_context::executor_type>>(
//            asio::make_work_guard(io_context))};
    std::shared_ptr<asio::executor_work_guard<asio::io_context::executor_type>>
        worker{new asio::executor_work_guard(asio::make_work_guard(io_context))};

    worker.reset();

    io_context.run();

    std::cout << "We will not see this line in console window. :(\n";

    return 0;
}
