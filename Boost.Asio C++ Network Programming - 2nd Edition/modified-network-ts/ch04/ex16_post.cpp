#include <functional>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

#include "asio.hpp"

using namespace std::chrono_literals;


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


size_t fac(size_t n)
{
    if (1 >= n)
        return n;
    std::this_thread::sleep_for(1000ms);
    return n * fac(n - 1);
}

void calculate_factorial(size_t n)
{
    global_stream_lock.lock();
    std::cout << "Calculating " << n << "! factorial\n";
    global_stream_lock.unlock();

    size_t f = fac(n);

    global_stream_lock.lock();
    std::cout << n << "! = " << f << '\n';
    global_stream_lock.unlock();
}


int main()
{
//    auto io_context{std::make_shared<asio::io_context>()};
    std::shared_ptr<asio::io_context> io_context{new asio::io_context};
//    auto worker{std::make_shared<
//        asio::executor_work_guard<asio::io_context::executor_type>>(
//            asio::make_work_guard(*io_context))};
    std::shared_ptr<asio::executor_work_guard<asio::io_context::executor_type>>
        worker{new asio::executor_work_guard{asio::make_work_guard(*io_context)}};

    global_stream_lock.lock();
    std::cout << "The program will exit once all work has finished.\n";
    global_stream_lock.unlock();

    using ThreadGroup = std::vector<std::thread>;
    ThreadGroup threads;
    for (auto i{0}; 5 != i; ++i)
        threads.emplace_back(std::thread{worker_thread, io_context, i});

    asio::post(*io_context, std::bind(calculate_factorial, 5));
    asio::post(*io_context, std::bind(calculate_factorial, 6));
    asio::post(*io_context, std::bind(calculate_factorial, 7));

    worker.reset();

    for (auto& thread : threads)
        thread.join();

    return 0;
}
