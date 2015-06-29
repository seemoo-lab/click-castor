#ifndef CLICK_XCAST_CASTOR_CHECK_CONTINUOUS_FLOW_HH
#define CLICK_XCAST_CASTOR_CHECK_CONTINUOUS_FLOW_HH

#include <click/element.hh>
#include "castor.hh"
#include "castor_next_flow_table.hh"
#include "crypto/crypto.hh"
#include "castor_routing_table.hh"

CLICK_DECLS

class CastorXcastCheckContinuousFlow: public Element {
public:
	CastorXcastCheckContinuousFlow() : fidTable(NULL), routingTable(NULL), crypto(NULL) {}

	const char *class_name() const { return "CastorXcastCheckContinuousFlow"; }
	const char *port_count() const { return PORTS_1_1; }
	const char *processing() const { return PUSH; }
	int configure(Vector<String>&, ErrorHandler*);

	void push(int, Packet*);
private:
	CastorNextFlowTable* fidTable;
	CastorRoutingTable* routingTable;
	Crypto* crypto;
};

CLICK_ENDDECLS

#endif
