#ifndef CLICK_CASTOR_TIMEOUT_TABLE_HH
#define CLICK_CASTOR_TIMEOUT_TABLE_HH

#include <click/element.hh>
#include <click/hashtable.hh>
#include "../castor.hh"
#include "castor_timeout.hh"
#include "../neighbor_id.hh"

CLICK_DECLS

class CastorTimeoutTable : public Element {
public:
	const char *class_name() const { return "CastorTimeoutTable"; }
	const char *port_count() const { return PORTS_0_0; }
	const char *processing() const { return AGNOSTIC; }
	int configure(Vector<String>&, ErrorHandler*);

	CastorTimeout& getTimeout(const FlowId& flow, const NeighborId& forwarder);

private:
	typedef HashTable<NeighborId, CastorTimeout> ForwarderEntry;
	typedef HashTable<FlowId, ForwarderEntry> FlowEntry;
	FlowEntry flows;
};

CLICK_ENDDECLS

#endif
