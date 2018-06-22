#include <functional>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

#include "asio.hpp"

using namespace std::chrono_literals;


std::mutex global_stream_lock;

void worker_thread(std::shared_ptr<asio::io_context> io_context)
{
    global_stream_lock.lock();
    std::cout << "Thread start.\n";
    global_stream_lock.unlock();

    io_context->run();

    global_stream_lock.lock();
    std::cout << "Thread finist.\n";
    global_stream_lock.unlock();
}

void dispatch(int i)
{
    global_stream_lock.lock();
    std::cout << "dispatch() function for i = " << i << '\n';
    global_stream_lock.unlock();
}

void post(int i)
{
    global_stream_lock.lock();
    std::cout << "post() function for i = " << i << '\n';
    global_stream_lock.unlock();
}

void running(std::shared_ptr<asio::io_context> io_context)
{
    for (auto x{0}; 5 != x; ++x) {
//        asio::dispatch(*io_context, std::bind(dispatch, x));
//        asio::post(*io_context, std::bind(post, x));
        asio::post(*io_context, std::bind(post, x));
        asio::dispatch(*io_context, std::bind(dispatch, x));
        std::this_thread::sleep_for(1000ms);
    }
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
    std::cout << "The program will exit automatically once all work has finished.\n";
    global_stream_lock.unlock();

    using ThreadGroup = std::vector<std::thread>;
    ThreadGroup threads;
    threads.emplace_back(std::thread{worker_thread, io_context});
    asio::post(*io_context, std::bind(running, io_context));

    worker.reset();
    for (auto& thread : threads)
        thread.join();

    return 0;
}
