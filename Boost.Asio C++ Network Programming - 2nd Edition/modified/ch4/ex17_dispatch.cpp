#include <functional>
#include <iostream>
#include <thread>
#include <memory>
#include <mutex>
#include <vector>

#include "asio.hpp"

using namespace std::chrono_literals;


std::mutex global_stream_lock;


void worker_thread(std::shared_ptr<asio::io_service> io_srv)
{
    global_stream_lock.lock();
    std::cout << "Thread Start.\n";
    global_stream_lock.unlock();

    io_srv->run();

    global_stream_lock.lock();
    std::cout << "Thread Finish.\n";
    global_stream_lock.unlock();
}

void dispatch(int i)
{
    global_stream_lock.lock();
    std::cout << "dispatch() Function for i = " << i << '\n';
    global_stream_lock.unlock();
}

void post(int i)
{
    global_stream_lock.lock();
    std::cout << "post() Function for i = " << i << '\n';
    global_stream_lock.unlock();
}

void running(std::shared_ptr<asio::io_service> io_srv)
{
    for (auto x{0}; 5 != x; ++x) {
        /*io_srv->dispatch(std::bind(dispatch, x));
        io_srv->post(std::bind(post, x));*/
        io_srv->post(std::bind(post, x));
        io_srv->dispatch(std::bind(dispatch, x));
        std::this_thread::sleep_for(1000ms);
    }
}

int main()
{
    auto io_srv{std::make_shared<asio::io_service>()};
    auto worker{std::make_shared<asio::io_service::work>(*io_srv)};

    global_stream_lock.lock();
    std::cout << "The program will exit automatically once all work has finished\n";
    global_stream_lock.unlock();

    std::vector<std::thread> threads;
    for (auto i{0}; 1 != i; ++i)
        threads.emplace_back(std::thread{worker_thread, io_srv});

    io_srv->post(std::bind(running, io_srv));
    worker.reset();

    for (auto& t : threads)
        t.join();

    return 0;
}
