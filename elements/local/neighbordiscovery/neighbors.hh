#ifndef CLICK_NEIGHBORS_HH
#define CLICK_NEIGHBORS_HH

#include <click/element.hh>
#include <click/hashtable.hh>
#include <click/list.hh>
#include <click/timer.hh>
#include <click/timestamp.hh>
#include "neighbor_beacon.hh"

CLICK_DECLS

class Neighbors: public Element {
public:
	inline Neighbors() : timer(this), timeout(1000), enabled(true) {}

	const char *class_name() const { return "Neighbors"; }
	const char *port_count() const { return PORTS_0_0; }
	const char *processing() const { return AGNOSTIC; }
	int configure(Vector<String>&, ErrorHandler*);

	/**
	 * Inserts the neighbor. If an entry already exists, it is updated with the current timestamp.
	 */
	void addNeighbor(NodeId neighbor);

	/**
	 * Whether such a neighbor exists and it has not yet timed out.
	 */
	inline bool hasNeighbor(NodeId neighbor) { return !enabled || neighbors.get_pointer(neighbor) != NULL; }

	/**
	 * Returns the number of neighbors that have not yet timed out.
	 */
	inline unsigned int neighborCount() const { return neighbors.size(); }

    void add_handlers();

private:
	struct ListNode {
		inline ListNode(NodeId neighbor, Timestamp timeout) : neighbor(neighbor), timeout(timeout) {}
		List_member<ListNode> node;
		NodeId neighbor;
		Timestamp timeout;
	};

public:
	/**
	 * A const iterator over all non-expired nodes in the current neighbor set.
	 * Do not use outside the scope of a Click packet push/pull function or the iterator may no longer be valid.
	 */
	class const_iterator {
	public:
		inline const_iterator(HashTable<NodeId, ListNode *>::const_iterator it) { this->it = it; }
		inline bool operator==(const_iterator it) { return this->it == it.it; }
		inline bool operator!=(const_iterator it) { return this->it != it.it; }
		inline void operator++(int) { it++; }

		inline const NodeId &entry() const { return it.key(); }

	private:
		HashTable<NodeId, ListNode *>::const_iterator it;
	};

	inline const_iterator begin() const { const_iterator it(neighbors.begin()); return it; }
	inline const_iterator end() const { const_iterator it = neighbors.end(); return it; }

private:
	void run_timer(Timer*);

	List<ListNode, &ListNode::node> timeout_queue;
	Timer timer;

	HashTable<NodeId, ListNode *> neighbors;

	unsigned int timeout;

	static String read_handler(Element*, void*);

	struct Statistics {
		enum {
			num, // current number of neighbors
		};
	};

	bool enabled;
};

CLICK_ENDDECLS

#endif
