#ifndef CLICK_CASTOR_ROUTING_TABLE_HH
#define CLICK_CASTOR_ROUTING_TABLE_HH

#include <click/element.hh>
#include "../castor.hh"
#include "../neighbor_id.hh"
#include "castor_estimator.hh"
#include "../util/ephemeral_map.hh"

CLICK_DECLS

class CastorRoutingTable : public Element {
public:
	typedef HashTable<NeighborId, CastorEstimator> FlowEntry;
	typedef ephemeral_map<FlowId, FlowEntry>::size_type size_type;

public:
	CastorRoutingTable() : default_entry(CastorEstimator(0)) {};
	~CastorRoutingTable() {
		delete flows;
	}

	const char *class_name() const { return "CastorRoutingTable"; }
	const char *port_count() const { return PORTS_0_0; }
	const char *processing() const { return AGNOSTIC; }
	int configure(Vector<String>&, ErrorHandler*);

	FlowEntry& at(const Hash &flow) { return flows->at_or_default(flow, default_entry); };
	void insert(const Hash &flow, const FlowEntry &entry);
	size_type count(const Hash &flow) const;

	void add_handlers();

private:
	void run_timer(Timer *timer) { flows->run_timer(timer); }

	ephemeral_map<FlowId, FlowEntry> *flows;

	double updateDelta;

	FlowEntry default_entry;

	String unparse(const FlowId&) const;
	void print(const FlowId&) const;

	static String read_table_handler(Element *e, void *);
};

CLICK_ENDDECLS

#endif
