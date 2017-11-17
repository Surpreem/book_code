#include <cstdint>
#include <exception>
#include <functional>
#include <iostream>
#include <list>
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
    std::cout << "Theread " << counter << " Start.\n";
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

class ClientContext : public std::enable_shared_from_this<ClientContext> {
public:
    ClientContext(asio::io_service& io_srv)
        : socket{io_srv}, recv_buffer_index{0}
    {
        recv_buffer.resize(4906);
    }

    ~ClientContext()
    {}

    void close()
    {
        std::error_code ec;
        socket.shutdown(asio::ip::tcp::socket::shutdown_both, ec);
        socket.close(ec);
    }

    void on_send(
        std::list<std::vector<uint8_t>>::iterator iter,
        std::error_code const& ec)
    {
        if (ec) {
            global_stream_lock.lock();
            std::cout << "on_send Error: " << ec << ".\n";
            global_stream_lock.unlock();
            close();
        } else {
            global_stream_lock.lock();
            std::cout << "Sent " << iter->size() << " bytes.\n";
            global_stream_lock.unlock();
        }
        send_buffer.erase(iter);

        // Start the next pending send
        if (!send_buffer.empty()) {
            asio::async_write(
                socket,
                asio::buffer(send_buffer.front()),
                std::bind(
                    &ClientContext::on_send,
                    shared_from_this(),
                    send_buffer.begin(),
                    std::placeholders::_1));
        }
    }

    void send(void const* buffer, size_t length)
    {
        auto can_send_now{false};
        std::vector<uint8_t> output;
        std::copy(
            static_cast<uint8_t const*>(buffer),
            static_cast<uint8_t const*>(buffer) + length,
            std::back_inserter(output));

        // Store if this is the only current send or not
        can_send_now = send_buffer.empty();

        // Save the buffer to be sent
        send_buffer.push_back(output);

        // Only send if there are no more pending buffers waiting!
        if (can_send_now) {
            // Start the next pending send
            asio::async_write(
                socket,
                asio::buffer(send_buffer.front()),
                std::bind(
                    &ClientContext::on_send,
                    shared_from_this(),
                    send_buffer.begin(),
                    std::placeholders::_1));
        }
    }

    void on_recv(std::error_code const& ec, size_t bytes_transferred)
    {
        if (ec) {
            global_stream_lock.lock();
            std::cout << "on_recv Error: " << ec << ".\n";
            global_stream_lock.unlock();
            close();
        } else {
            // Increase how many bytes we have saved up
            recv_buffer_index += bytes_transferred;

            // Debug information
            global_stream_lock.lock();
            std::cout << "Recv: " << bytes_transferred << " bytes.\n";
            global_stream_lock.unlock();

            // Dump all the data
            global_stream_lock.lock();
            for (size_t x{0}; recv_buffer_index != x; ++x) {
                std::cout << static_cast<char>(recv_buffer[x]) << " ";
                if (0 == (x + 1) % 16) {
                    std::cout << '\n';
                }
            }
            std::cout << '\n' << std::dec;
            global_stream_lock.unlock();

            // Clear all the data
            recv_buffer_index = 0;

            // Start the next recive cycle
            recv();
        }
    }

    void recv()
    {
        socket.async_read_some(
            asio::buffer(
                &recv_buffer[recv_buffer_index],
                recv_buffer.size() - recv_buffer_index),
            std::bind(
                &ClientContext::on_recv,
                shared_from_this(),
                std::placeholders::_1,
                std::placeholders::_2));
    }

    asio::ip::tcp::socket socket;
    std::vector<uint8_t> recv_buffer;
    size_t recv_buffer_index;
    std::list<std::vector<uint8_t>> send_buffer;
};

void on_accept(std::shared_ptr<ClientContext> client, std::error_code const& ec)
{
    if (ec) {
        global_stream_lock.lock();
        std::cout << "on_accept Error: " << ec << ".\n";
        global_stream_lock.unlock();
    } else {
        global_stream_lock.lock();
        std::cout << "Accepted!" << ".\n";
        global_stream_lock.unlock();

        // 2 bytes message size, followed by the message
        client->send("Hi there!", 9);
        client->recv();
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

    // We just use one worker thread
    // in order that no thread safety issues
    std::vector<std::thread> threads;
    for (auto i{0}; 1 != i; ++i)
        threads.emplace_back(std::thread{worker_thread, io_srv, i});

    auto acceptor{std::make_shared<asio::ip::tcp::acceptor>(*io_srv)};
    auto client{std::make_shared<ClientContext>(*io_srv)};

    try {
        asio::ip::tcp::resolver resolver{*io_srv};
        asio::ip::tcp::resolver::query query{
            "127.0.0.1", std::to_string(4444)};
        asio::ip::tcp::endpoint endpoint{*resolver.resolve(query)};
        acceptor->open(endpoint.protocol());
        acceptor->set_option(asio::ip::tcp::acceptor::reuse_address{false});
        acceptor->bind(endpoint);
        acceptor->listen(asio::socket_base::max_connections);
        acceptor->async_accept(
            client->socket,
            std::bind(on_accept, client, std::placeholders::_1));

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

    io_srv->stop();

    for (auto& t : threads)
        t.join();

    return 0;
}
