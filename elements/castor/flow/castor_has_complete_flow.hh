#ifndef CLICK_CASTOR_HAS_COMPLETE_FLOW_HH
#define CLICK_CASTOR_HAS_COMPLETE_FLOW_HH

#include <click/element.hh>
#include "castor_flow_table.hh"

CLICK_DECLS

class CastorHasCompleteFlow: public Element {
public:
	const char *class_name() const { return "CastorHasCompleteFlow"; }
	const char *port_count() const { return PORTS_1_1X2; }
	const char *processing() const { return PROCESSING_A_AH; }
	int configure(Vector<String>&, ErrorHandler*);

	Packet* simple_action(Packet *);
private:
	CastorFlowTable* flowtable;
};

CLICK_ENDDECLS

#endif
