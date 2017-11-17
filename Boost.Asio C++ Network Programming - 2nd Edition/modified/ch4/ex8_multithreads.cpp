#include <iostream>
#include <memory>
#include <thread>
#include <vector>

#include "asio.hpp"


asio::io_service io_srv;
auto a{0};

void worker_thread()
{
    std::cout << ++a << ".\n";
    io_srv.run();
    std::cout << "End.\n";
}

int main()
{
    auto worker{std::make_shared<asio::io_service::work>(io_srv)};
    std::cout << "Press ENTER key to exit!\n";

    std::vector<std::thread> threads;
    for (auto i{0}; 5 != i; ++i)
        threads.emplace_back(std::thread{worker_thread});

    std::cin.get();
    io_srv.stop();
    for (auto& t : threads)
        t.join();

    return 0;
}
