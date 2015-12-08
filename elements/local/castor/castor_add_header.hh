#ifndef CLICK_CASTOR_ADD_HEADER_HH
#define CLICK_CASTOR_ADD_HEADER_HH

#include <click/element.hh>
#include "castor.hh"
#include "flow/castor_flow_manager.hh"

CLICK_DECLS

/**
 * Add Castor header to IP packet
 * TODO: Should replace IP header instead of add
 */
class CastorAddHeader: public Element {
public:
	const char *class_name() const { return "CastorAddHeader"; }
	const char *port_count() const { return PORTS_1_1; }
	const char *processing() const { return AGNOSTIC; }
	int configure(Vector<String>&, ErrorHandler*);

	Packet* simple_action(Packet*);
private:
	CastorFlowManager* flow;
};

CLICK_ENDDECLS

#endif
