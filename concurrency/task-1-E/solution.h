#pragma once

#include <cstddef>
#include <iostream>
#include <thread>
#include <atomic>
#include <vector>
#include <cmath>
#include <array>

/* ! Класс -- мьютекс Петерсона. Реализация -- как на лекции. */
class CPetersonMutex {
 public:
  CPetersonMutex() {
        want[0].store(false);
        want[1].store(false);
        victim.store(0);
    }
    void lock(int t) {
        want[t].store(true);
        victim.store(t);
        while (want[1 - t].load() && victim.load() == t) {
            /* ! Чтобы тред во сне не мешал исполняться активному треду. */
            std::this_thread::yield();
        }
    }
    void unlock(int t) {
        want[t].store(false);
    }
    
 private:
    std::array<std::atomic<bool>, 2> want;
    std::atomic<int> victim;
};

/* ! Класс -- древесный мьютекс. Реализация -- храним в виде кучи во внешних узлах
    locks, а в листьях - threads. 
*/
class TreeMutex {
public:
    /* ! Дополняем размер до степени двойки. Выделяем место для heap of locks. */
    TreeMutex(std::size_t num_threads) {
        height = ceil(log2(num_threads));
        num_tree_nodes = pow(2, height);
        locks = std::vector<CPetersonMutex> (num_tree_nodes - 1);
    }

    /* ! Поднимаемся от листьев к корню. На каждом уровне кучи за мьютекс соревнуются
        сыновья текущего узла.
    */
    void lock(std::size_t current_thread) {
        std::size_t winner = current_thread + num_tree_nodes - 1;
        for (std::size_t i = 0; i < height; ++i) {
            int now_locked = 1 - (winner & 1);
            winner = (winner - 1) >> 1;
            locks[winner].lock(now_locked);
        }
    }

    /* ! Спускаемся от корня к листу. На каждом уровне вызываем unlock для треда,
        который был заблокирован во время lock.
    */
    void unlock(std::size_t current_thread) {
        /* ! Соответсвенно, в обратном порядке вызываем unlock */
        std::size_t index = 0;
        for (int i = height - 1; i >= 0; --i) {
            locks[index].unlock((current_thread >> i) % 2);
            /* ! Идем туда, где мы разблокировали */
            index = index * 2 + 1 + (current_thread >> i) % 2;
        }
    }
private:
    std::vector<CPetersonMutex> locks;
    std::size_t num_tree_nodes;
    double height;
};