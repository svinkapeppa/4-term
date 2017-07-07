#ifndef TASK_SOLUTION_H
#define TASK_SOLUTION_H

#include <thread>
#include <atomic>

class ticket_lock{
public:
    void init();
    void lock();
    void unlock();
    bool try_lock();
private:
    std::atomic<int> owner_ticket;
    std::atomic<int> next_ticket;
};

#endif //TASK_SOLUTION_H
