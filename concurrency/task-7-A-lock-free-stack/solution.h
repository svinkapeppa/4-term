#pragma once

#include <atomic>
#include <thread>

///////////////////////////////////////////////////////////////////////

template <typename T>
class LockFreeStack {
    struct Node {
        T element_{};
        std::atomic<Node*> next_{nullptr};

        explicit Node(T element, Node* next = nullptr)
        	: element_(std::move(element))
        	, next_(next) {
		}

		explicit Node() {
		}
    };

 public:
    explicit LockFreeStack() {
    }

    ~LockFreeStack() {
    	Destroy(top_.load());
    	Destroy(deleted_.load());
    }

    void Push(T item) {
        Node* new_top = new Node{item};

        Node* curr_top = top_.load();
        new_top->next_.store(curr_top);

        while(!top_.compare_exchange_weak(curr_top, new_top)) {
        	new_top->next_.store(curr_top);
        }
    }

    bool Pop(T& item) {
        Node* curr_top = top_.load();

        while(true) {
        	if (!curr_top) {
        		return false;
        	}
        	if (top_.compare_exchange_weak(curr_top, curr_top->next_.load())) {
        		item = curr_top->element_;
        		PushDeleted(curr_top);
        		return true;
        	}
        }
    }

 private:
 	void PushDeleted(Node* curr_top) {
 		Node* curr_deleted = deleted_.load();
 		curr_top->next_.store(curr_deleted);

 		while(!deleted_.compare_exchange_weak(curr_deleted, curr_top)) {
 			curr_top->next_.store(curr_deleted);
 		}
 	}

 	void Destroy(Node* item) {
 		while (item) {
	 		Node* curr_item = item;
	 		item = item->next_.load();
	 		delete curr_item;
	 	}
 	}

    std::atomic<Node*> top_{nullptr};
    std::atomic<Node*> deleted_{nullptr};
};

///////////////////////////////////////////////////////////////////////

template <typename T>
using ConcurrentStack = LockFreeStack<T>;

///////////////////////////////////////////////////////////////////////
