#include <conio.h>

#include <iostream>
#include <mutex>
#include <thread>

#include "wrapper.h"

using namespace std::chrono_literals;


std::mutex global_stream_lock;


class MyConnection : public Connection {
public:
    MyConnection(std::shared_ptr<Hive> hive) : Connection{hive}
    {}

    ~MyConnection()
    {}

private:
    void on_accept(std::string const& host, uint16_t port) override
    {
        global_stream_lock.lock();
        std::cout << "[on_accept] " << host << ":" << port << '\n';
        global_stream_lock.unlock();

        recv();
    }

    void on_connect(std::string const& host, uint16_t port) override
    {
        global_stream_lock.lock();
        std::cout << "[on_connect] " << host << ":" << port << '\n';
        global_stream_lock.unlock();

        recv();

        std::string str{"GET / HTTP/1.0\r\n\r\n"};

        std::vector<uint8_t> request;
        std::copy(str.begin(), str.end(), std::back_inserter(request));
        send(request);
    }

    void on_send(std::vector<uint8_t> const& buffer) override
    {
        global_stream_lock.lock();
        std::cout << "[on_send] " << buffer.size() << " bytes\n";
        for (size_t x{0}; buffer.size() != x; ++x) {
            std::cout << buffer[x];
            if (0 == (x + 1) % 16)
                std::cout << '\n';
        }
        std::cout << '\n';
        global_stream_lock.unlock();
    }

    void on_recv(std::vector<uint8_t> const& buffer) override
    {
        global_stream_lock.lock();
        std::cout << "[on_recv] " << buffer.size() << " bytes\n";
        for (size_t x{0}; buffer.size() != x; ++x) {
            std::cout << buffer[x];
            if (0 == (x + 1) % 16)
                std::cout << '\n';
        }
        std::cout << '\n';
        global_stream_lock.unlock();

        recv();
    }

    void on_timer(std::chrono::duration<double> const& delta) override
    {
        global_stream_lock.lock();
        std::cout << "[on_timer] " << delta.count() << '\n';
        global_stream_lock.unlock();
    }

    void on_error(std::error_code const& error) override
    {
        global_stream_lock.lock();
        std::cout << "[on_error] " << error << '\n';
        global_stream_lock.unlock();
    }
};


int main()
{
    auto hive{std::make_shared<Hive>()};
    auto connection{std::make_shared<MyConnection>(hive)};
    connection->connect("www.packtpub.com", 80);

    while (!_kbhit()) {
        hive->poll();
        std::this_thread::sleep_for(1ms);
    }

    hive->stop();

    return 0;
}
