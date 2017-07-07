#pragma once

#include <cstddef>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <vector>

class Semaphore {
public:
	Semaphore (std::size_t num=0) 
		: threshold_(num) {
	}
	void Wait() {
		std::unique_lock<std::mutex> lock(mtx_);
		cv_.wait(lock, [&]{return threshold_;});
		--threshold_;
	}
	void Signal() {
		std::unique_lock<std::mutex> lock(mtx_);
		++threshold_;
		cv_.notify_one();
	}
private:
	std::mutex mtx_;
	std::size_t threshold_;
	std::condition_variable cv_;
};

class Robot {
public:
    Robot(const std::size_t num_foots) 
    	: foots_(num_foots - 1)
    	, sem_(num_foots) {
    	sem_[0].Signal();
    }
    ~Robot() {
    }
    void Step(const std::size_t foot) {
    	sem_[foot].Wait();
    	std::cout << "foot " << foot << std::endl;
    	sem_[Next(foot)].Signal();
    }
private:
	std::size_t Next(std::size_t i) {
		if (i == foots_) {
			return 0;
		} else {
			return i + 1;
		}
	}
	std::size_t foots_;
	std::vector<Semaphore> sem_;
};
