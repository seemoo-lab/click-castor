#ifndef CLICK_CASTOR_ROUTING_TABLE_HH
#define CLICK_CASTOR_ROUTING_TABLE_HH

#include <click/element.hh>
#include <click/hashtable.hh>
#include "../castor.hh"
#include "../neighbor_id.hh"
#include "castor_estimator.hh"

CLICK_DECLS

class CastorRoutingTable : public Element {
public:
	typedef HashTable<NeighborId, CastorEstimator> FlowEntry;

public:
	CastorRoutingTable() : flows(FlowEntry(CastorEstimator(0.0))) {};

	const char *class_name() const { return "CastorRoutingTable"; }
	const char *port_count() const { return PORTS_0_0; }
	const char *processing() const { return AGNOSTIC; }
	int configure(Vector<String>&, ErrorHandler*);

	FlowEntry& entry(const Hash& flow);
	FlowEntry& copy_estimators(const Hash& flow, const NodeId& src, const NodeId& dst);
	CastorEstimator& estimator(const Hash& flow, const NeighborId& forwarder);
	void update(const Hash& flow, const NodeId& src, const NodeId& dst);

	void add_handlers();
private:
	HashTable<FlowId, FlowEntry> flows;

	HashTable<Pair<NodeId, NodeId>, Hash> srcdstmap;
	HashTable<             NodeId , Hash>    dstmap;

	String unparse(const FlowId&) const;
	void print(const FlowId&) const;

	static String read_table_handler(Element *e, void *);
};

CLICK_ENDDECLS

#endif
