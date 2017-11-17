#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include "asio.hpp"

std::mutex global_stream_lock;


void worker_thread(std::shared_ptr<asio::io_service> io_srv, int counter)
{
    global_stream_lock.lock();
    std::cout << counter << ".\n";
    global_stream_lock.unlock();

    io_srv->run();

    global_stream_lock.lock();
    std::cout << "End.\n";
    global_stream_lock.unlock();
}


int main()
{
    auto io_srv{std::make_shared<asio::io_service>()};
    auto worker{std::make_shared<asio::io_service::work>(*io_srv)};

    std::cout << "Press ENTER key to exit!\n";
    std::vector<std::thread> threads;
    for (auto i{0}; 5 != i; ++i)
        //threads.emplace_back(std::thread{std::bind(worker_thread, io_srv, i)});
        threads.emplace_back(std::thread{worker_thread, io_srv, i});

    std::cin.get();
    io_srv->stop();
    for (auto& t : threads)
        t.join();

    return 0;
}
