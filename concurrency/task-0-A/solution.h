#include <iostream>
#include <vector>
#include <thread>

template <class Task>
void hello_world(std::function<void(const Task&)> func, const std::vector<Task>& tasks) {
    std::vector<std::thread> threads;
    for (const auto& it : tasks) {
        std::thread thread (func, it);
        threads.push_back(std::move(thread));
    }
    for (auto&& it : threads) {
        it.join();
    }
}