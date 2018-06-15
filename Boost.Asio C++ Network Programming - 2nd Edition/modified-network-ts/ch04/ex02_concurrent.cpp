#include <chrono>
#include <iostream>
#include <thread>
#include <vector>


void print1()
{
    for (auto i{0}; 5 != i; ++i) {
//        using namespace std::chrono_literals;
//        std::this_thread::sleep_for(500ms);
        std::this_thread::sleep_for(std::chrono::milliseconds{500});
        std::cout << "[print1] Line: " << i << '\n';
    }
}

void print2()
{
    for (auto i{0}; 5 != i; ++i) {
//        using namespace std::chrono_literals;
//        std::this_thread::sleep_for(500ms);
        std::this_thread::sleep_for(std::chrono::milliseconds{500});
        std::cout << "[print2] Line: " << i << '\n';
    }
}


int main()
{
    using ThreadGroup = std::vector<std::thread>;
    ThreadGroup threads;
    threads.emplace_back(std::thread(print1));
    threads.emplace_back(std::thread(print2));

    for (auto& thread : threads) {
        thread.join();
    }

    return 0;
}
