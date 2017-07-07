#pragma once

#include <functional>
#include <future>
#include <iostream>
#include <condition_variable>
#include <queue>
#include <mutex>
#include <stdexcept>
#include <atomic>
#include <utility>
#include <numeric>

template <class T, class Container = std::deque<T>>
class BlockingQueue {
public:
	explicit BlockingQueue(const size_t& capacity) 
		: max_num_threads_(capacity)
		, alive_(true) {
	}

  // You need to implement Put method which will block and wait while capacity
  // of your container exceeded. It should work with noncopyable objects and
  // use move semantics for placing elements in the container. If the queue is 
  // shutdown you need to throw an exception inherited from std::exceptiono.
	void Put(T&& el) {
		std::unique_lock<std::mutex> lock(mtx_);
		full_.wait(lock, [&](){return !alive_ || queue_.size() < max_num_threads_;});
		if (alive_) {
			queue_.push_back(std::move(el));
			empty_.notify_one();
		} else {
			throw std::exception();
		}
	}

  // Get should block and wait while the container is empty, if the queue
  // shutdown you will need to return false.
	bool Get(T& result) {
		std::unique_lock<std::mutex> lock(mtx_);
		empty_.wait(lock, [&](){return queue_.size() != 0 || !alive_;});
		if (!alive_ && queue_.size() == 0) {
			return false;
		} else {
			result = std::move(queue_.front());
			queue_.pop_front();
			full_.notify_one();
			return true;
		}
	}

  // Shutdown should turn off the queue and notify every thread which is waiting
  // for something to stop wait.
	void Shutdown() {
		std::unique_lock<std::mutex> lock(mtx_);
		alive_ = false;
		full_.notify_all();
		empty_.notify_all();
	}
private:
	Container queue_;
	size_t max_num_threads_;
	bool alive_;
	std::condition_variable empty_;
	std::condition_variable full_;
	std::mutex mtx_;
};

template <class T>
class ThreadPool {
public:
	ThreadPool() 
		: working_(true) {
			size_t num_threads = default_num_workers();
			num_workers_.store(num_threads);
			for (size_t i = 0; i < num_threads; ++i) {
				thread_workers_.emplace_back(&ThreadPool::work, this);
			}
	}

  	explicit ThreadPool(const size_t num_threads) 
  		: working_(true) 
  		, num_workers_(num_threads) {
	  		for (size_t i = 0; i < num_threads; ++i) {
	  			thread_workers_.emplace_back(&ThreadPool::work, this);
			}
	}

  	std::future<T> Submit(std::function<T()> task) {
  		std::packaged_task<T()> new_task(task);
  		auto returning = new_task.get_future();
  		queue_.Put(std::move(new_task));
  		return returning;
  	}

  	void Shutdown() {
  		if (working_.load()) {
  			working_.store(false);
  			std::unique_lock<std::mutex> lock(mtx_);
	  		queue_.Shutdown();
	  		ready_.wait(lock, [&](){return num_workers_.load() == 0;});
	  		for (size_t i = 0; i < thread_workers_.size(); ++i) {
	  			thread_workers_[i].join();
	  		}
	  	}
  	}

  	~ThreadPool() {
  		Shutdown();
  	}

  	ThreadPool(const ThreadPool& other) = delete;

  	ThreadPool & operator=(const ThreadPool&) = delete;
private:
	size_t default_num_workers() {
  		size_t provided = std::thread::hardware_concurrency();
  		if (provided) {
  			return provided;
  		} else {
  			return 4;
  		}
  	}

  	void work() {
  		while (true) {
	  		std::packaged_task<T()> task;
	  		if (queue_.Get(task)) {
	  			task();
	  		} else {
	  			--num_workers_;
	  			if (num_workers_.load() == 0) {
	  				ready_.notify_one();
	  			}
	  			break;
	  		}
	  	}
  	}

  	std::mutex mtx_;
  	std::atomic<bool> working_;
  	std::atomic<size_t> num_workers_;
  	std::condition_variable ready_;
	std::vector<std::thread> thread_workers_;
	BlockingQueue<std::packaged_task<T()>> queue_{std::numeric_limits<size_t>::max()};
};