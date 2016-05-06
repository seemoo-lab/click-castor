#ifndef CLICK_CASTOR_ROUTING_TABLE_HH
#define CLICK_CASTOR_ROUTING_TABLE_HH

#include <click/element.hh>
#include <click/hashtable.hh>
#include "castor.hh"
#include "../neighbordiscovery/neighbor_id.hh"
#include "castor_estimator.hh"

CLICK_DECLS

class CastorRoutingTable : public Element {
public:
	typedef NodeId SubflowId;

	CastorRoutingTable() : flows(SubflowEntry(ForwarderEntry(CastorEstimator(0.0)))) { };

	const char *class_name() const { return "CastorRoutingTable"; }
	const char *port_count() const { return PORTS_0_0; }
	const char *processing() const { return AGNOSTIC; }
	int configure(Vector<String>&, ErrorHandler*);

	HashTable<NeighborId, CastorEstimator>& entry(const Hash& flow, const SubflowId& subflow);
	HashTable<NeighborId, CastorEstimator>& entry_copy(const Hash& flow, const NodeId& src, const NodeId& dst);
	CastorEstimator& estimator(const Hash& flow, const SubflowId& subflow, const NeighborId& forwarder);
	void update(const Hash& flow, const NodeId& src, const NodeId& dst);

	void add_handlers();
private:
	typedef HashTable<NeighborId, CastorEstimator> ForwarderEntry;
	typedef HashTable<SubflowId, ForwarderEntry> SubflowEntry;
	typedef HashTable<FlowId, SubflowEntry> FlowEntry;

	FlowEntry flows;

	HashTable<Pair<NodeId, NodeId>, Hash> srcdstmap;
	HashTable<             NodeId , Hash>    dstmap;

	String unparse(const FlowId&, const SubflowId&) const;
	void print(const FlowId&, const SubflowId&) const;

	static String read_table_handler(Element *e, void *);
};

CLICK_ENDDECLS

#endif
