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

void timer_handler(
    std::shared_ptr<asio::steady_timer> timer,
    std::shared_ptr<asio::io_service::strand> strand,
    std::error_code const& ec)
{
    if (ec) {
        global_stream_lock.lock();
        std::cout << "Error Message: " << ec << ".\n";
        global_stream_lock.unlock();
    } else {
        global_stream_lock.lock();
        std::cout << "You see this every three seconds.\n";
        global_stream_lock.unlock();

        timer->expires_from_now(1s);
        timer->async_wait(/*strand->wrap*/(
            std::bind(timer_handler, timer, strand, std::placeholders::_1)));
    }
}

void print(int number)
{
    std::cout << "Number: " << number << '\n';
    std::this_thread::sleep_for(500ms);
}


int main()
{
    auto io_srv{std::make_shared<asio::io_service>()};
    auto worker{std::make_shared<asio::io_service::work>(*io_srv)};
    auto strand{std::make_shared<asio::io_service::strand>(*io_srv)};

    global_stream_lock.lock();
    std::cout << "Press ENTER to exit!\n";
    global_stream_lock.unlock();

    std::vector<std::thread> threads;
    for (auto i{0}; 5 != i; ++i)
        threads.emplace_back(std::thread{worker_thread, io_srv, i});

    std::this_thread::sleep_for(1s);

    strand->post(std::bind(print, 1));
    strand->post(std::bind(print, 2));
    strand->post(std::bind(print, 3));
    strand->post(std::bind(print, 4));
    strand->post(std::bind(print, 5));

    // https://github.com/chriskohlhoff/asio/issues/59
    // auto timer{std::make_shared<asio::deadline_timer>(*io_srv)};
    auto timer{std::make_shared<asio::steady_timer>(*io_srv)};
    timer->expires_from_now(1s);
    timer->async_wait(/*strand->wrap*/(
        std::bind(timer_handler, timer, strand, std::placeholders::_1)));

    std::cin.get();

    io_srv->stop();

    for (auto& t : threads)
        t.join();

    return 0;
}
