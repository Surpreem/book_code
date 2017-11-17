#include <exception>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
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


int main()
{
    auto io_srv{std::make_shared<asio::io_service>()};
    auto worker{std::make_shared<asio::io_service::work>(*io_srv)};
    auto strand{std::make_shared<asio::io_service::strand>(*io_srv)};

    global_stream_lock.lock();
    std::cout << "Press ENTER to exit!\n";
    global_stream_lock.unlock();

    std::vector<std::thread> threads;
    for (auto i{0}; 2 != i; ++i)
        threads.emplace_back(std::thread{worker_thread, io_srv, i});

    asio::ip::tcp::socket socket{*io_srv};

    try {
        asio::ip::tcp::resolver resolver{*io_srv};
        asio::ip::tcp::resolver::query query{
            "www.packtpub.com", std::to_string(80)};
        asio::ip::tcp::resolver::iterator iter{resolver.resolve(query)};
        asio::ip::tcp::endpoint endpoint{*iter};

        global_stream_lock.lock();
        std::cout << "Connecting to: " << endpoint << '\n';
        global_stream_lock.unlock();

        socket.connect(endpoint);
        std::cout << "Connected!\n";
    } catch (std::exception& ex) {
        global_stream_lock.lock();
        std::cout << "Message: " << ex.what() << ".\n";
        global_stream_lock.unlock();
    }

    std::cin.get();

    std::error_code ec;
    socket.shutdown(asio::ip::tcp::socket::shutdown_both, ec);
    socket.close(ec);

    io_srv->stop();

    for (auto& t : threads)
        t.join();

    return 0;
}
