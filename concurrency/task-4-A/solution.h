#pragma once

#include <forward_list>
#include <functional>
#include <algorithm>
#include <vector>
#include <atomic>
#include <mutex>

template <typename T, class Hash = std::hash<T>>
class StripedHashSet {
 public:
    explicit StripedHashSet(const size_t concurrency_level,
                            const size_t growth_factor = 3,
                            const double load_factor = 0.75)
    : load_factor_(load_factor),
      growth_factor_(growth_factor) {
      	capacity_.store(0);
      	locks_ = std::vector<std::mutex>(concurrency_level);
      	table_ = std::vector<std::forward_list<T>>(5 * concurrency_level);
    }

    bool Insert(const T& element) {
        bool result = false;
        size_t element_hash_value = Hash()(element);
        std::unique_lock<std::mutex> lock(locks_[GetStripeIndex(element_hash_value)]);
        if (table_.size() * load_factor_ <= capacity_.load()) {
        	lock.unlock();
        	Resize();
        	lock.lock();
        }
        auto&& bucket = table_[GetBucketIndex(element_hash_value)];
        if (std::find(bucket.begin(), bucket.end(),	element) == bucket.end()) {
        	result = true;
        	capacity_.fetch_add(1);
        	bucket.push_front(element);
        }
        return result;
    }

    bool Remove(const T& element) {
        bool result = false;
        size_t element_hash_value = Hash()(element);
        std::unique_lock<std::mutex> lock(locks_[GetStripeIndex(element_hash_value)]);
        auto&& bucket = table_[GetBucketIndex(element_hash_value)];
        if (std::find(bucket.begin(), bucket.end(),	element) != bucket.end()) {
        	result = true;
        	bucket.remove(element);
        	capacity_.fetch_sub(1);
        }
        return result;
    }

    bool Contains(const T& element) {
        bool result = false;
        size_t element_hash_value = Hash()(element);
        std::unique_lock<std::mutex> lock(locks_[GetStripeIndex(element_hash_value)]);
        auto&& bucket = table_[GetBucketIndex(element_hash_value)];
        if (std::find(bucket.begin(), bucket.end(),	element) != bucket.end()) {
        	result = true;
        }
        return result;
    }

    size_t Size() {
        return capacity_.load();
    }

 private:
    size_t GetBucketIndex(const size_t element_hash_value) const {
        return element_hash_value % table_.size();
    }

    size_t GetStripeIndex(const size_t element_hash_value) const {
        return element_hash_value % locks_.size(); 
    }
    void Resize() {
        std::vector<std::unique_lock<std::mutex>> locks;
        locks.emplace_back(locks_[0]);
    	if (table_.size() * load_factor_ > capacity_.load()) {
    		return;
    	}
    	for (size_t i = 1; i < locks_.size(); ++i) {
    		locks.emplace_back(locks_[i]);
    	}
		size_t newCapacity = growth_factor_ * table_.size();
		std::vector<std::forward_list<T>> instance(newCapacity);
		std::swap(instance, table_);
		for (size_t i = 0; i < instance.size(); ++i) {
			for (auto jt : instance[i]) {
				table_[GetBucketIndex(Hash()(jt))].push_front(jt);
			}
		}
    }
    std::atomic<size_t> capacity_;
    double load_factor_;
    size_t growth_factor_;
    std::vector<std::mutex> locks_;
    std::vector<std::forward_list<T>> table_;
};

template <typename T> using ConcurrentSet = StripedHashSet<T>;
