#include <functional>
#include <iostream>
#include <memory>
#include <thread>

#include "asio.hpp"


void worker_thread(std::shared_ptr<asio::io_context> io_context, int counter)
{
    std::cout << counter << ".\n";
    io_context->run();
    std::cout << "End.\n";
}


int main()
{
    std::shared_ptr<asio::io_context> io_context{new asio::io_context};
//    auto worker{std::make_shared<
//        asio::executor_work_guard<asio::io_context::executor_type>>(
//            asio::make_work_guard(io_context))};
    std::shared_ptr<asio::executor_work_guard<asio::io_context::executor_type>>
        worker{new asio::executor_work_guard{asio::make_work_guard(*io_context)}};

    std::cout << "Press ENTER key to exit!\n";

    using ThreadGroup = std::vector<std::thread>;
    ThreadGroup threads;
    for (auto i{0}; 5 != i; ++i)
        threads.emplace_back(std::thread{worker_thread, io_context, i});

    std::cin.get();

    io_context->stop();
    for (auto& thread : threads)
        thread.join();

    return 0;
}
