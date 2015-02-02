#ifndef CLICK_CASTOR_XCAST_SET_FIXED_HEADER_HH
#define CLICK_CASTOR_XCAST_SET_FIXED_HEADER_HH

#include <click/element.hh>
#include "castor.hh"
#include "castor_flow_stub.hh"

CLICK_DECLS

/**
 * Sets Castor Xcast header to IP packet
 * TODO: Should replace IP header instead of add
 */
class CastorXcastSetFixedHeader: public Element {
public:
	CastorXcastSetFixedHeader() : cflow(0) {}
		
	const char *class_name() const { return "CastorXcastSetFixedHeader"; }
	const char *port_count() const { return PORTS_1_1; }
	const char *processing() const { return PUSH; }
	int configure(Vector<String>&, ErrorHandler*);

	void push(int, Packet*);
private:
	CastorFlowStub* cflow;
};

CLICK_ENDDECLS

#endif
