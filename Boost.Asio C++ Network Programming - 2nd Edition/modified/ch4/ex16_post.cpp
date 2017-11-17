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
    std::cout << counter << ".\n";
    global_stream_lock.unlock();

    io_srv->run();

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

    auto f{fac(n)};

    global_stream_lock.lock();
    std::cout << n << "! = " << f << '\n';
    global_stream_lock.unlock();
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

    io_srv->post(std::bind(calculate_factorial, 5));
    io_srv->post(std::bind(calculate_factorial, 6));
    io_srv->post(std::bind(calculate_factorial, 7));

    worker.reset();
    for (auto& t : threads)
        t.join();

    return 0;
}
