#include "solution.h"
#include <iostream>

void ticket_lock::init() {
	owner_ticket.store(0);
	next_ticket.store(0);
}

void ticket_lock::lock() {
	std::atomic<int> this_thread_ticket((next_ticket.fetch_add(1)));
	while (this_thread_ticket != owner_ticket) {
        std::cout << "sleep\n";
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

void ticket_lock::unlock() {
	owner_ticket.fetch_add(1);
}

bool ticket_lock::try_lock() {
	int oldVal = owner_ticket.load();
    return next_ticket.compare_exchange_strong(oldVal, oldVal + 1);
}