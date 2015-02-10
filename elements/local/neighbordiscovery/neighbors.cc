#include <click/config.h>
#include <click/confparse.hh>
#include "neighbors.hh"

CLICK_DECLS

int Neighbors::configure(Vector<String>& conf, ErrorHandler* errh) {
	int result = cp_va_kparse(conf, this, errh,
			"TIMEOUT", cpkP + cpkM, cpUnsigned, &timeout,
			"ENABLE", cpkP, cpBool, &enabled,
			cpEnd);
	if (enabled) timer.initialize(this);
	return result;
}

void Neighbors::run_timer(Timer*) {
	ListNode *node;

	// Fetch expired neighbors from the timer queue and remove them from the neighbors set
	for (;;) {
		node = timeout_queue.front();
		if (node == NULL)
			return;
		if (Timestamp::recent_steady() - node->timeout < 0)
			break;
		timeout_queue.pop_front();
		neighbors.erase(node->neighbor);
		delete node;
	}

	// Restart the timer with the timeout of the first expiring neighbor
	timer.schedule_at_steady(node->timeout);
}

void Neighbors::addNeighbor(NodeId neighbor) {
	// Compute the neighbor's timeout based on the current "steady" time
	// We use steady time because, in difference to the system time, it is unaffected by user clock changes
	Timestamp node_timeout = Timestamp::recent_steady() + Timestamp::make_msec(timeout);

	// Look for neighbor in neighbors set
	ListNode **node_ptr = neighbors.get_pointer(neighbor);

	// If found, update the neighbor's timeout and put it back in the timeout queue
	if (node_ptr != NULL) {
		ListNode *node = *node_ptr;
		bool isFront = node == timeout_queue.front();
		timeout_queue.erase(node);
		node->timeout = node_timeout;
		timeout_queue.push_back(node);
		if (isFront) {
			node = timeout_queue.front();
			timer.unschedule();
			timer.schedule_at_steady(node->timeout);
		}
	}

	// Otherwise, add the neighbor to the neighbors set and create a new entry in the timeout queue
	else {
		ListNode *node = new ListNode(neighbor, node_timeout);
		if (node != NULL) {
			neighbors.set(neighbor, node);
			bool empty = timeout_queue.empty();
			timeout_queue.push_back(node);
			// Start the timer if it has been inactive
			if (empty)
				timer.schedule_at_steady(node_timeout);
		}
	}
}

String Neighbors::read_handler(Element *e, void *thunk) {
	Neighbors* neighbors = static_cast<Neighbors*>(e);

	switch(reinterpret_cast<uintptr_t>(thunk)) {
	case Statistics::num:
		return String(neighbors->neighborCount());
	default:
		click_chatter("enum error");
		return String();
	}
}

void Neighbors::add_handlers() {
	add_read_handler("num", read_handler, Statistics::num);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(Neighbors)
