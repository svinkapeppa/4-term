#pragma once

#include "arena_allocator.h"

#include <atomic>
#include <limits>
#include <mutex>

///////////////////////////////////////////////////////////////////////

template <typename T>
struct ElementTraits {
    static T Min() {
        return std::numeric_limits<T>::min();
    }
    static T Max() {
        return std::numeric_limits<T>::max();
    }
};

///////////////////////////////////////////////////////////////////////

class SpinLock {
public:
    explicit SpinLock() {
        locked_.clear();
    }

    void Lock() {
        while(locked_.test_and_set()){
        	// wait
        }
    }

    void Unlock() {
        locked_.clear();
    }


    // adapters for BasicLockable concept

    void lock() {
        Lock();
    }

    void unlock() {
        Unlock();
    }

private:
	std::atomic_flag locked_;
};

///////////////////////////////////////////////////////////////////////

template <typename T>
class OptimisticLinkedSet {
private:
    struct Node {
        T element_;
        std::atomic<Node*> next_;
        SpinLock lock_{};
        std::atomic<bool> marked_{false};

        Node(const T& element, Node* next = nullptr)
            : element_(element),
              next_(next) {
        }
    };

    struct Edge {
        Node* pred_;
        Node* curr_;

        Edge(Node* pred, Node* curr)
            : pred_(pred),
              curr_(curr) {
        }
    };

public:
    explicit OptimisticLinkedSet(ArenaAllocator& allocator)
        : allocator_(allocator),
          size_(0) {
        CreateEmptyList();
    }

    bool Insert(const T& element) {
    	while (true) {
	    	Edge edge = Locate(element);
	    	std::unique_lock<SpinLock> pred_lock(edge.pred_->lock_);
	    	std::unique_lock<SpinLock> curr_lock(edge.curr_->lock_);
	    	if (Validate(edge)) {
	    		if (edge.curr_->element_ == element) {
	    			return false;
	    		} else {
	    			Node* entry = allocator_.New<Node>(element);
	    			entry->next_.store(edge.curr_);
	    			edge.pred_->next_.store(entry);
	    			size_.fetch_add(1);
	    			return true;
	    		}
	    	}
	    }
    }

    bool Remove(const T& element) {
    	while (true) {
    		Edge edge = Locate(element);
	    	std::unique_lock<SpinLock> pred_lock(edge.pred_->lock_);
	    	std::unique_lock<SpinLock> curr_lock(edge.curr_->lock_);
	    	if (Validate(edge)) {
	    		if (edge.curr_->element_ != element) {
	    			return false;
	    		} else {
	    			edge.curr_->marked_ = true;
	    			edge.pred_->next_.store(edge.curr_->next_);
	    			size_.fetch_sub(1);
	    			return true;
	    		}
	    	}
    	}
    }

    bool Contains(const T& element) const {
        Node* curr = head_;
        while (curr->element_ < element) {
        	curr = curr->next_;
        }
        return curr->element_ == element
        	&& !curr->marked_;
    }

    size_t Size() const {
        return size_.load();
    }

private:
    void CreateEmptyList() {
        head_ = allocator_.New<Node>(ElementTraits<T>::Min());
        head_->next_ = allocator_.New<Node>(ElementTraits<T>::Max());
    }

    Edge Locate(const T& element) const {
    	Node* pred = head_;
    	Node* curr = head_->next_;
    	while (curr->element_ < element) {
    		pred = curr;
    		curr = curr->next_;
    	}
        return Edge{pred, curr};
    }

    bool Validate(const Edge& edge) const {
        return !edge.pred_->marked_
        	&& !edge.curr_->marked_
        	&& edge.pred_->next_ == edge.curr_;
    }

private:
    ArenaAllocator& allocator_;
    Node* head_{nullptr};
    std::atomic<size_t> size_;
};

template <typename T> using ConcurrentSet = OptimisticLinkedSet<T>;

///////////////////////////////////////////////////////////////////////
