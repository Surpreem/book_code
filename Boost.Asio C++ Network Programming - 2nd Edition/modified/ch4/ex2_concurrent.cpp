#include <chrono>
#include <iostream>
#include <thread>

using namespace std::chrono_literals;


void print1()
{
    for (auto i{0}; i != 5; ++i) {
        std::this_thread::sleep_for(500ms);
        std::cout << "[print1] Line: " << i << '\n';
    }
}

void print2()
{
    for (auto i{0}; i != 5; ++i) {
        std::this_thread::sleep_for(500ms);
        std::cout << "[print2] Line: " << i << '\n';
    }
}


int main()
{
    std::thread t1(print1);
    std::thread t2(print2);
    t1.join();
    t2.join();

    return 0;
}
