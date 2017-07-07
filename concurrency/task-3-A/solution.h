#include <iostream>
#include <condition_variable>
#include <queue>
#include <mutex>
#include <stdexcept>
#include <atomic>
#include <utility>

template <class T, class Container = std::deque<T>>
class BlockingQueue {
public:
	explicit BlockingQueue(const size_t& capacity) {
		max_num_threads_ = capacity;
		alive_.store(true);
	}

  // You need to implement Put method which will block and wait while capacity
  // of your container exceeded. It should work with noncopyable objects and
  // use move semantics for placing elements in the container. If the queue is 
  // shutdown you need to throw an exception inherited from std::exceptiono.
	void Put(T&& el) {
		std::unique_lock<std::mutex> lock(mtx_);
		while (queue_.size() >= max_num_threads_&& alive_.load()) {
			full_.wait(lock);
		}
		if (alive_.load()) {
			queue_.push_back(std::move(el));
			empty_.notify_all();
		} else {
			throw std::exception();
		}
	}

  // Get should block and wait while the container is empty, if the queue
  // shutdown you will need to return false.
	bool Get(T& result) {
		std::unique_lock<std::mutex> lock(mtx_);
		empty_.wait(lock, [&](){return queue_.size() != 0 || !alive_.load();});
		if (!alive_.load() && queue_.empty()) {
			return false;
		} else {
			result = std::move(queue_.front());
			queue_.pop_front();
			full_.notify_all();
			return true;
		}
	}

  // Shutdown should turn off the queue and notify every thread which is waiting
  // for something to stop wait.
	void Shutdown() {
		std::unique_lock<std::mutex> lock(mtx_);
		alive_.store(false);
		full_.notify_all();
		empty_.notify_all();
	}
private:
	Container queue_;
	size_t max_num_threads_;
	std::atomic<bool> alive_;
	std::condition_variable empty_;
	std::condition_variable full_;
	std::mutex mtx_;
};
