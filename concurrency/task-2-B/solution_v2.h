#pragma once

#include <cstddef>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <atomic>

class Robot {
public:
	Robot()
		: mode_(-1) {
	  
	}
    void StepLeft() {
    	std::unique_lock<std::mutex> lock(mtx_);
    	left_.wait(lock, [&]{return mode_ == -1;});
    	mode_ = 1;
    	right_.notify_all();
    	std::cout << "left" << std::endl;
    }
    void StepRight() {
    	std::unique_lock<std::mutex> lock(mtx_);
    	right_.wait(lock, [&]{return mode_ == 1;});
	    mode_ = -1;
	    left_.notify_all();
    	std::cout << "right" << std::endl;
    }
private:
	std::mutex mtx_;
	int mode_;
	std::condition_variable left_;
	std::condition_variable right_;
};