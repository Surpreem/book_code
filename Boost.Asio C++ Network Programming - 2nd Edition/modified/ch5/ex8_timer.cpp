#include <exception>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <system_error>
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

    while (true) {
        try {
            std::error_code ec;
            io_srv->run(ec);
            if (ec) {
                global_stream_lock.lock();
                std::cout << "Message: " << ec << ".\n";
                global_stream_lock.unlock();
            }
            break;
        } catch (std::exception& ex) {
            global_stream_lock.lock();
            std::cout << "Message: " << ex.what() << ".\n";
            global_stream_lock.unlock();
        }
    }

    global_stream_lock.lock();
    std::cout << "Thread " << counter << " End.\n";
    global_stream_lock.unlock();
}

void timer_handler(std::error_code const& ec)
{
    if (ec) {
        global_stream_lock.lock();
        std::cout << "Error Message: " << ec << ".\n";
        global_stream_lock.unlock();
    } else {
        global_stream_lock.lock();
        std::cout << "You see this line because you have waited"
            " for 10 seconds.\n";
        std::cout << "Now press ENTER to exit.\n";
        global_stream_lock.unlock();
    }
}


int main()
{
    auto io_srv{std::make_shared<asio::io_service>()};
    auto worker{std::make_shared<asio::io_service::work>(*io_srv)};

    global_stream_lock.lock();
    std::cout << "Wait for 10 seconds to see what happen, ";
    std::cout << "otherwise press ENTER to exit!\n";
    global_stream_lock.unlock();

    std::vector<std::thread> threads;
    for (auto i{0}; 5 != i; ++i)
        threads.emplace_back(std::thread{worker_thread, io_srv, i});

    // https://github.com/chriskohlhoff/asio/issues/59
    // asio::deadline_timer timer(*io_srv);
    asio::steady_timer timer(*io_srv);
    timer.expires_from_now(10s);
    timer.async_wait(timer_handler);

    std::cin.get();

    io_srv->stop();

    for (auto& t : threads)
        t.join();

    return 0;
}
