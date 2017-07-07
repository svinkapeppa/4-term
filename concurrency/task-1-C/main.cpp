#include <iostream>
#include "solution.h"

int main() {
    ticket_lock tl;
    tl.init();
    tl.lock();
    int k = 0;
    for (int i = 0; i < 1000; ++i) {
        ++k;
    }
    if (tl.try_lock()) {
        std::cout << "WASN'T LOCKED\n";
    } else {
        std::cout << "ALREADY LOCKED\n";
    }
    if (tl.try_lock()) {
        std::cout << "WASN'T LOCKED\n";
    } else {
        std::cout << "ALREADY LOCKED\n";
    }
    if (tl.try_lock()) {
        std::cout << "WASN'T LOCKED\n";
    } else {
        std::cout << "ALREADY LOCKED\n";
    }
    if (tl.try_lock()) {
        std::cout << "WASN'T LOCKED\n";
    } else {
        std::cout << "ALREADY LOCKED\n";
    }
    tl.unlock();
    if (tl.try_lock()) {
        std::cout << "WASN'T LOCKED\n";
    } else {
        std::cout << "ALREADY LOCKED\n";
    }
    if (tl.try_lock()) {
        std::cout << "WASN'T LOCKED\n";
    } else {
        std::cout << "ALREADY LOCKED\n";
    }
    if (tl.try_lock()) {
        std::cout << "WASN'T LOCKED\n";
    } else {
        std::cout << "ALREADY LOCKED\n";
    }
    tl.unlock();
    tl.lock();
    tl.unlock();
}