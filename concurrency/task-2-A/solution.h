#pragma once

#include <condition_variable>
#include <cstddef>
#include <mutex>

template <typename ConditionVariable = std::condition_variable>
class CyclicBarrier {
public:
    CyclicBarrier(size_t num_threads)
    : numberOfThreads_(num_threads),
      treshold_(num_threads),
      mode_(1) {
      
    }
    void Pass() {
        size_t local = mode_;
        std::unique_lock<std::mutex> lock(mtx_);
        if (!--treshold_) {
            mode_ *= -1;
            treshold_ = numberOfThreads_;
            cv_.notify_all();
        } else {
            cv_.wait(lock, [&]{return local != mode_;});
        }
    }
private:
    size_t numberOfThreads_;
    ConditionVariable cv_;
    size_t treshold_;
    std::mutex mtx_;
    size_t mode_;
};
