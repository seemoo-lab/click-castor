/*
 * Copyright (c) 2016 Milan Schmittner
 *
 * This file is part of click-castor.
 *
 * click-castor is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * click-castor is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with click-castor.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CLICK_EPHEMERAL_MAP_HH
#define CLICK_EPHEMERAL_MAP_HH

#include <click/element.hh>
#include <click/hashtable.hh>
#include <click/list.hh>
#include <click/timer.hh>
#include <click/timestamp.hh>

CLICK_DECLS

template <typename K, typename V>
class ephemeral_map {
public:
	typedef size_t size_type;

	struct ListNode {
		inline ListNode(const K &key, const V &value, const Timestamp &timeout) : key(key), value(value), timeout(timeout) {}
		List_member<ListNode> node;
		K key;
		V value;
		Timestamp timeout;
	};

public:
	ephemeral_map(const Timestamp &timeout, const Timestamp &clean_interval, const V &default_value, Element *owner)
			: timeout(timeout), clean_interval(clean_interval), timer(owner), default_value(default_value), owner(owner), _map(NULL) {
		timer.initialize(owner);
	}

	V& at(const K &key) { return at_or_default(key, default_value); };
	V& at_or_default(const K &, const V &);
	const V& at_notouch(const K &key) const;
	HashTable<K, ListNode *> map() { return _map; }
	void insert(const K &key, const V &value) { (void) at_or_default(key, value); }
	size_type count(const K &key) const { return _map.count(key); }
	size_type size() const { return _map.size(); }

	void run_timer(Timer*);

private:
	Timestamp timeout;
	Timestamp clean_interval;
	Timer timer;

	V default_value;

	Element *owner;

	List<ListNode, &ListNode::node> _timeout_queue;
	HashTable<K, ListNode *> _map;
};

template<typename K, typename V>
V& ephemeral_map<K,V>::at_or_default(const K &key, const V &init) {
	Timestamp node_timeout = Timestamp::recent_steady() + timeout;

	ListNode **node_ptr = _map.get_pointer(key);
	ListNode *node = NULL;
	if (node_ptr == NULL) {
		node = new ListNode(key, init, node_timeout);
		_map.set(key, node);
		bool empty = _timeout_queue.empty();
		_timeout_queue.push_back(node);
		// Start the timer if it has been inactive
		if (empty)
			timer.schedule_at_steady(node_timeout);
	} else {
		node = *node_ptr;
		bool isFront = node == _timeout_queue.front();
		_timeout_queue.erase(node);
		node->timeout = node_timeout;
		_timeout_queue.push_back(node);
		if (isFront) {
			timer.unschedule();
			timer.schedule_at_steady(_timeout_queue.front()->timeout);
		}
	}
	return node->value;
}

template<typename K, typename V>
const V& ephemeral_map<K,V>::at_notouch(const K &key) const {
	ListNode * const *node_ptr = _map.get_pointer(key);
	if (node_ptr == NULL) {
		return default_value;
	} else {
		return (*node_ptr)->value;
	}

}

template<typename K, typename V>
void ephemeral_map<K,V>::run_timer(Timer* _timer) {
#ifndef MAX
	#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif
	assert(_timer == &timer);

	ListNode *node;

	unsigned int removed = 0;
	// Fetch expired route entries from the timer queue and remove them from the routing table
	for (;; ++removed) {
		node = _timeout_queue.front();
		if (node == NULL)
			break;
		if (Timestamp::recent_steady() - node->timeout < 0)
			break;
		_timeout_queue.pop_front();
		_map.erase(node->key);
		delete node;
	}

	click_chatter("[%s] CLEAN: delete %u, left %u", owner->declaration().c_str(), removed, _map.size());

	if (node != NULL) {
		// Restart the timer with the timeout of the first expiring value
		Timestamp next_clean_time = MAX(node->timeout, Timestamp::recent_steady() + clean_interval);
		timer.schedule_at_steady(next_clean_time);
	} else {
		_map.~HashTable();
		new (&_map) HashTable<K, ListNode *>();
	}
}

CLICK_ENDDECLS

#endif //CLICK_EPHEMERAL_MAP_HH
