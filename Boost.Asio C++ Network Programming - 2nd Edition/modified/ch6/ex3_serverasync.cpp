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
        } catch (std::exception &ex) {
            global_stream_lock.lock();
            std::cout << "Message: " << ex.what() << ".\n";
            global_stream_lock.unlock();
        }
    }

        global_stream_lock.lock();
        std::cout << "Thread " << counter << " End.\n";
        global_stream_lock.unlock();
}

void on_accept(
    std::shared_ptr<asio::ip::tcp::socket> socket, std::error_code const& ec)
{
    if (ec) {
        global_stream_lock.lock();
        std::cout << "on_accept Error: " << ec << ".\n";
        global_stream_lock.unlock();
    } else {
        global_stream_lock.lock();
        std::cout << "Accepted!\n";
        global_stream_lock.unlock();
    }
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

    auto acceptor{std::make_shared<asio::ip::tcp::acceptor>(*io_srv)};
    auto socket{std::make_shared<asio::ip::tcp::socket>(*io_srv)};

    try {
        asio::ip::tcp::resolver resolver{*io_srv};
        asio::ip::tcp::resolver::query query {
            "127.0.0.1", std::to_string(4444)};
        asio::ip::tcp::endpoint endpoint{*resolver.resolve(query)};
        acceptor->open(endpoint.protocol());
        acceptor->set_option(asio::ip::tcp::acceptor::reuse_address{false});
        acceptor->bind(endpoint);
        acceptor->listen(asio::socket_base::max_connections);
        acceptor->async_accept(
            *socket, std::bind(on_accept, socket, std::placeholders::_1));

        global_stream_lock.lock();
        std::cout << "Listening on: " << endpoint << '\n';
        global_stream_lock.unlock();
    } catch (std::exception& ex) {
        global_stream_lock.lock();
        std::cout << "Message: " << ex.what() << ".\n";
        global_stream_lock.unlock();
    }

    std::cin.get();

    std::error_code ec;
    acceptor->close(ec);

    socket->shutdown(asio::ip::tcp::socket::shutdown_both, ec);
    socket->close(ec);

    io_srv->stop();

    for (auto& t : threads)
        t.join();

    return 0;
}
