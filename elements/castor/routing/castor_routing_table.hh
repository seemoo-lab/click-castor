#ifndef CLICK_CASTOR_ROUTING_TABLE_HH
#define CLICK_CASTOR_ROUTING_TABLE_HH

#include <click/element.hh>
#include <click/hashtable.hh>
#include <click/list.hh>
#include <click/timer.hh>
#include <click/timestamp.hh>
#include "../castor.hh"
#include "../neighbor_id.hh"
#include "castor_estimator.hh"

CLICK_DECLS

class CastorRoutingTable : public Element {
public:
	typedef HashTable<NeighborId, CastorEstimator> FlowEntry;
	typedef size_t size_type;
public:
	CastorRoutingTable() : timer(this), default_entry(CastorEstimator(updateDelta)) {};

	const char *class_name() const { return "CastorRoutingTable"; }
	const char *port_count() const { return PORTS_0_0; }
	const char *processing() const { return AGNOSTIC; }
	int configure(Vector<String>&, ErrorHandler*);

	FlowEntry& at(const Hash &flow) { return at_or_default(flow, default_entry); };
	void insert(const Hash &flow, const FlowEntry &entry);
	size_type count(const Hash &flow) const;
	size_type size() const;
	void add_handlers();

private:
	FlowEntry& at_or_default(const Hash &flow, const FlowEntry &init);

	void run_timer(Timer*);

	struct ListNode {
		inline ListNode(const FlowId &id, const FlowEntry &entry, const Timestamp &timeout) : id(id), entry(entry), timeout(timeout) {}
		List_member<ListNode> node;
		FlowId id;
		FlowEntry entry;
		Timestamp timeout;
	};

	List<ListNode, &ListNode::node> timeout_queue;
	Timer timer;
	unsigned int timeout;
	unsigned int clean_interval;

	HashTable<FlowId, ListNode *> flows;
	double updateDelta;

	FlowEntry default_entry;

	String unparse(const FlowId&) const;
	void print(const FlowId&) const;

	static String read_table_handler(Element *e, void *);
};

CLICK_ENDDECLS

#endif
