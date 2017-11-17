#include <functional>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

#include "asio.hpp"


void worker_thread(std::shared_ptr<asio::io_service> iosrv, int counter)
{
    std::cout << counter << ".\n";
    iosrv->run();
    std::cout << "End.\n";
}


int main()
{
    auto io_srv{std::make_shared<asio::io_service>()};
    auto worker{std::make_shared<asio::io_service::work>(*io_srv)};
    std::cout << "Press ENTER key to exit!\n";

    std::vector<std::thread> threads;
    for (auto i{0}; 5 != i; ++i)
        //threads.emplace_back(std::thread{std::bind(worker_thread, io_srv, i)});
        threads.emplace_back(std::thread{worker_thread, io_srv, i});

    std::cin.get();
    io_srv->stop();
    for (auto& t : threads)
        t.join();

    return 0;
}
