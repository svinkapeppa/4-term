#pragma once

#include "spinlock_pause.h"

#include <iostream>
#include <atomic>
#include <thread>

//////////////////////////////////////////////////////////////////////

/* scalable and fair MCS (Mellor-Crummy, Scott) spinlock implementation
 *
 * usage:
 * {
 *   MCSSpinLock::Guard guard(spinlock); // spinlock acquired
 *   ... // in critical section
 * } // spinlock released
 */

template <template <typename T> class Atomic = std::atomic>
class MCSSpinLock {
public:
  class Guard {
  public:
    explicit Guard(MCSSpinLock& spinlock)
      : spinlock_(spinlock) {
        Acquire();
      }

      ~Guard() {
        Release();
      }

  private:
    void Acquire() {
	  // add self to spinlock queue and wait for ownership

      Guard* prev_tail = spinlock_.wait_queue_tail_.exchange(this);
      if (prev_tail == nullptr) {
        return;
      }
      is_owner_.store(false);
      prev_tail->next_.store(this);
      while (!is_owner_.load()) {
        asm volatile("rep; nop" ::: "memory");
      }
    }

    void Release() {
      /* transfer ownership to the next guard node in spinlock wait queue
       * or reset tail pointer if there are no other contenders
       */

      if (next_.load() == nullptr) {
        Guard* prev_tail = spinlock_.wait_queue_tail_.exchange(nullptr);
        if (prev_tail == this) {
          return;
        }
        Guard* deleted = spinlock_.wait_queue_tail_.exchange(prev_tail);
        while (next_.load() == nullptr) {
          asm volatile("rep; nop" ::: "memory");
        }
        if (deleted != nullptr) {
          deleted->next_.store(next_.load());
        } else {
          next_.load()->is_owner_.store(true);
        }
      } else {
        next_.load()->is_owner_.store(true);
      }
    }

  private:
    MCSSpinLock& spinlock_;

    Atomic<bool> is_owner_{true};
    Atomic<Guard*> next_{nullptr};
  };

private:
  Atomic<Guard*> wait_queue_tail_{nullptr};
};

/////////////////////////////////////////////////////////////////////

// alias for checker
template <template <typename T> class Atomic = std::atomic>
using SpinLock = MCSSpinLock<Atomic>;

/////////////////////////////////////////////////////////////////////
