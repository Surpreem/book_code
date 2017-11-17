#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <system_error>
#include <thread>
#include <vector>

#include "asio.hpp"


std::mutex global_stream_lock;


void worker_thread(std::shared_ptr<asio::io_service> io_srv, int counter)
{
    global_stream_lock.lock();
    std::cout << "Thread " << counter << " Start.\n";
    global_stream_lock.unlock();

    std::error_code ec;
    io_srv->run(ec);

    if (ec) {
        global_stream_lock.lock();
        std::cout << "Message: " << ec << ".\n";
        global_stream_lock.unlock();
    }

    global_stream_lock.lock();
    std::cout << "Thread " << counter << " End.\n";
    global_stream_lock.unlock();
}

void throw_exception(std::shared_ptr<asio::io_service> io_srv)
{
    global_stream_lock.lock();
    std::cout << "Throw Exception\n";
    global_stream_lock.unlock();

    io_srv->post(std::bind(throw_exception, io_srv));

    throw std::runtime_error{"The Exception !!!"};
}


int main()
{
    auto io_srv{std::make_shared<asio::io_service>()};
    auto worker{std::make_shared<asio::io_service::work>(*io_srv)};

    global_stream_lock.lock();
    std::cout << "The program will exit once all work has finished.\n";
    global_stream_lock.unlock();

    std::vector<std::thread> threads;
    for (auto i{0}; 5 != i; ++i)
        threads.emplace_back(std::thread{worker_thread, io_srv, i});

    io_srv->post(std::bind(throw_exception, io_srv));

    for (auto& t : threads)
        t.join();

    return 0;
}
