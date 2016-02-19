#ifndef CLICK_CASTOR_UPDATE_ESTIMATES_HH
#define CLICK_CASTOR_UPDATE_ESTIMATES_HH

#include <click/element.hh>
#include "castor_history.hh"
#include "castor_routing_table.hh"

CLICK_DECLS

class CastorUpdateEstimates: public Element {
public:
	const char *class_name() const { return "CastorUpdateEstimates"; }
	const char *port_count() const { return PORTS_1_1X2; }
	const char *processing() const { return PROCESSING_A_AH; }
	int configure(Vector<String>&, ErrorHandler*);

	Packet* simple_action(Packet*);
private:
	CastorRoutingTable* table;
	CastorHistory* history;
};

CLICK_ENDDECLS

#endif
