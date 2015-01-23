#ifndef CLICK_CASTOR_NEIGHBORS_HH
#define CLICK_CASTOR_NEIGHBORS_HH

#include <click/element.hh>
#include <click/hashtable.hh>
#include <click/list.hh>
#include <click/timer.hh>
#include <click/timestamp.hh>
#include "castor.hh"

CLICK_DECLS

typedef IPAddress Neighbor;

class CastorNeighbors: public Element {
public:
	inline CastorNeighbors() : timer(this), timeout(1000) {}

	const char *class_name() const { return "CastorNeighbors"; }
	const char *port_count() const { return PORTS_0_0; }
	const char *processing() const { return AGNOSTIC; }
	int configure(Vector<String>&, ErrorHandler*);

	/**
	 * Inserts the neighbor. If an entry already exists, it is updated with the current timestamp.
	 */
	void addNeighbor(Neighbor neighbor);

	/**
	 * Whether such a neighbor exists and it has not yet timed out.
	 */
	inline bool hasNeighbor(Neighbor neighbor) { return neighbors.get_pointer(neighbor) != NULL; }

	/**
	 * Returns the number of neighbors that have not yet timed out.
	 */
	inline unsigned int neighborCount() const { return neighbors.size(); }

private:
	struct ListNode {
		inline ListNode(Neighbor neighbor, Timestamp timeout) : neighbor(neighbor), timeout(timeout) {}
		List_member<ListNode> node;
		Neighbor neighbor;
		Timestamp timeout;
	};

public:
	/**
	 * A const iterator over all non-expired nodes in the current neighbor set.
	 * Do not use outside the scope of a Click packet push/pull function or the iterator may no longer be valid.
	 */
	class const_iterator {
	public:
		inline const_iterator(HashTable<Neighbor, ListNode *>::const_iterator it) { this->it = it; }
		inline bool operator==(const_iterator it) { return this->it == it.it; }
		inline bool operator!=(const_iterator it) { return this->it != it.it; }
		inline void operator++(int) { it++; }

		inline const Neighbor &entry() const { return it.key(); }

	private:
		HashTable<Neighbor, ListNode *>::const_iterator it;
	};

	inline const_iterator begin() const { const_iterator it(neighbors.begin()); return it; }
	inline const_iterator end() const { const_iterator it = neighbors.end(); return it; }

private:
	void run_timer(Timer*);

	List<ListNode, &ListNode::node> timeout_queue;
	Timer timer;

	HashTable<Neighbor, ListNode *> neighbors;

	unsigned int timeout;
};

CLICK_ENDDECLS

#endif
