#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include "asio.hpp"


std::mutex global_stream_lock;

void worker_thread(std::shared_ptr<asio::io_context> io_context, int counter)
{
    global_stream_lock.lock();
    std::cout << counter << ".\n";
    global_stream_lock.unlock();

    io_context->run();

    global_stream_lock.lock();
    std::cout << "End.\n";
    global_stream_lock.unlock();
}


int main()
{
    std::shared_ptr<asio::io_context> io_context{new asio::io_context};
//    auto worker{std::make_shared<
//        asio::executor_work_guard<asio::io_context::executor_type>>(
//            asio::make_work_guard(*io_context))};
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
