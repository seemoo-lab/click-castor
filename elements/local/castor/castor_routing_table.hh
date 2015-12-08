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

	HashTable<NeighborId, CastorEstimator>& getFlowEntry(const FlowId& flow, const SubflowId& subflow);

	CastorEstimator& getEstimator(const FlowId& flow, const SubflowId& subflow, const NeighborId& forwarder);

	/**
	 * Copies the forwarder entry of one flow to another.
	 * If an entry for (newFlow, subflow) already exists, that entry is not overwritten.
	 * Returns true if entry was copied, false otherwise.
	 */
	bool copyFlowEntry(const FlowId& newFlow, const FlowId& oldFlow, const SubflowId& subflow);

	void add_handlers();
private:
	typedef HashTable<NeighborId, CastorEstimator> ForwarderEntry;
	typedef HashTable<SubflowId, ForwarderEntry> SubflowEntry;
	typedef HashTable<FlowId, SubflowEntry> FlowEntry;

	FlowEntry flows;

	String str(const FlowId&, const SubflowId&);
	void print(const FlowId&, const SubflowId&);

	static String read_table_handler(Element *e, void *);
};

CLICK_ENDDECLS

#endif
