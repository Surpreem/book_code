#include <iostream>
#include <memory>
#include <vector>

#include "asio.hpp"


asio::io_context io_context;
auto a{0};


void worker_thread()
{
    std::cout << ++a << ".\n";
    io_context.run();
    std::cout << "End.\n";
}


int main()
{
//    auto worker{std::make_shared<
//        asio::executor_work_guard<asio::io_context::executor_type>>(
//            asio::make_work_guard(io_context))};
    std::shared_ptr<asio::executor_work_guard<asio::io_context::executor_type>>
        worker{new asio::executor_work_guard(asio::make_work_guard(io_context))};

    std::cout << "Press ENTER key to exit!\n";

    using ThreadGroup = std::vector<std::thread>;
    ThreadGroup threads;
    for (auto i{0}; 5 != i; ++i)
        threads.emplace_back(std::thread(worker_thread));

    std::cin.get();

    io_context.stop();
    for (auto& thread : threads)
        thread.join();

    return 0;
}
