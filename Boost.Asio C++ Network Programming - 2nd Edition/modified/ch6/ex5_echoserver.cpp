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
    }

    void on_send(std::vector<uint8_t> const& buffer) override
    {
        global_stream_lock.lock();
        std::cout << "[on_send] " << buffer.size() << " bytes\n";
        for (size_t x{0}; buffer.size() != x; ++x){
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
        send(buffer);
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


class Myacceptor : public Acceptor {
public:
    Myacceptor(std::shared_ptr<Hive> hive) : Acceptor{hive}
    {}

    ~Myacceptor()
    {}

private:
    bool on_accept(
        std::shared_ptr<Connection> connection,
        std::string const& host,
        uint16_t port) override
    {
        global_stream_lock.lock();
        std::cout << "[on_acceptor] " << host << ":" << port << '\n';
        global_stream_lock.unlock();

        return true;
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
    auto acceptor{std::make_shared<Myacceptor>(hive)};
    acceptor->listen("127.0.0.1", 4444);

    auto connection{std::make_shared<MyConnection>(hive)};
    acceptor->accept(connection);

    while (!_kbhit()) {
        hive->poll();
        std::this_thread::sleep_for(1ms);
    }

    hive->stop();

    return 0;
}
