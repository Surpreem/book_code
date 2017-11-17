#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include "asio.hpp"

using namespace std::chrono_literals;


std::mutex global_stream_lock;


void worker_thread(std::shared_ptr<asio::io_service> io_srv, int counter)
{
    global_stream_lock.lock();
    std::cout << "Thread " << counter << " Start.\n";
    global_stream_lock.unlock();

    io_srv->run();

    global_stream_lock.lock();
    std::cout << "Thread " << counter << " End.\n";
    global_stream_lock.unlock();
}

void print(int number)
{
    std::cout << "Number: " << number << '\n';
}


int main()
{
    auto io_srv{std::make_shared<asio::io_service>()};
    auto worker{std::make_shared<asio::io_service::work>(*io_srv)};
    asio::io_service::strand strand(*io_srv);

    global_stream_lock.lock();
    std::cout << "The program will exit once all work has been finished.\n";
    global_stream_lock.unlock();

    std::vector<std::thread> threads;
    for (auto i{0}; 5 != i; ++i)
        threads.emplace_back(std::thread{worker_thread, io_srv, i});

    std::this_thread::sleep_for(500ms);
    strand.post(std::bind(print, 1));
    strand.post(std::bind(print, 2));
    strand.post(std::bind(print, 3));
    strand.post(std::bind(print, 4));
    strand.post(std::bind(print, 5));

    worker.reset();
    for (auto& t : threads)
        t.join();

    return 0;
}
