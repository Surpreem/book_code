#include <exception>
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
    std::cout << "Thread " << counter << " Start.\n";
    global_stream_lock.unlock();

    try {
        io_srv->run();

        global_stream_lock.lock();
        std::cout << "Thread " << counter << "End.\n";
        global_stream_lock.unlock();
    } catch (std::exception& ex) {
        global_stream_lock.lock();
        std::cout << "Message: " << ex.what() << ".\n";
        global_stream_lock.unlock();
    }
}

void throw_exception(std::shared_ptr<asio::io_service> io_srv, int counter)
{
    global_stream_lock.lock();
    std::cout << "Throw Exception " << counter << "\n";
    global_stream_lock.unlock();

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
    for (auto i{0}; 2 != i; ++i)
        threads.emplace_back(std::thread{worker_thread, io_srv, i});

    io_srv->post(std::bind(throw_exception, io_srv, 1));
    io_srv->post(std::bind(throw_exception, io_srv, 2));
    io_srv->post(std::bind(throw_exception, io_srv, 3));
    io_srv->post(std::bind(throw_exception, io_srv, 4));
    io_srv->post(std::bind(throw_exception, io_srv, 5));

    for (auto& t : threads)
        t.join();

    return 0;
}
