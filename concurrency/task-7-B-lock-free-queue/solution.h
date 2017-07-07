#pragma once

#include <thread>
#include <atomic>

///////////////////////////////////////////////////////////////////////

template <typename T, template <typename U> class Atomic = std::atomic>
class LockFreeQueue {
  struct Node {
    T element_{};
    Atomic<Node*> next_{nullptr};

    explicit Node(T element, Node* next = nullptr)
      : element_(std::move(element))
      , next_(next) {
    }

    explicit Node() {
    }
  };

  public:
    explicit LockFreeQueue() 
      : opr_num_(0) {
        Node* dummy = new Node{};
        head_ = dummy;
        tail_ = dummy;
        junk_ = dummy;
    }

    ~LockFreeQueue() {
      ReclaimJunkNodes(nullptr);
    }

    void Enqueue(T item) {
      ++opr_num_;

      Node* new_tail = new Node{item};
      Node* curr_tail = tail_.load();

      while (true) {
      	curr_tail = tail_.load();
        Node* null = nullptr;

       	if (!curr_tail->next_.load()) {
    	  if (curr_tail->next_.compare_exchange_strong(null, new_tail)) {
            break;
          }
        } else {
          tail_.compare_exchange_strong(curr_tail, curr_tail->next_.load());
        }
      }

      tail_.compare_exchange_strong(curr_tail, new_tail);

      --opr_num_;
    }

    bool Dequeue(T& item) {
      ++opr_num_;

      while (true) {
       	Node* curr_head = head_.load();
       	Node* curr_tail = tail_.load();

       	if (curr_head == curr_tail) {
      	  if (!curr_head->next_.load()) {
            --opr_num_;
            return false;
          } else {
            tail_.compare_exchange_strong(curr_head, curr_head->next_.load());
          }
        } else {
          if (head_.compare_exchange_strong(curr_head, curr_head->next_.load())) {
            item = curr_head->next_.load()->element_;
            curr_head = head_.load();
            if (opr_num_.load() == 1) {
              ReclaimJunkNodes(curr_head);
            } else {
              --opr_num_;
            }
            return true;
          }
        }
      }
    }

  private:
    void ReclaimJunkNodes(Node* head) {
      while (junk_.load() != head) {
        Node* node = junk_.load();
        junk_.store(junk_.load()->next_.load());
        delete node;
      }
      --opr_num_;
    }

    Atomic<Node*> head_{nullptr};
    Atomic<Node*> tail_{nullptr};
    Atomic<Node*> junk_{nullptr};
    Atomic<int> opr_num_;
};

///////////////////////////////////////////////////////////////////////
