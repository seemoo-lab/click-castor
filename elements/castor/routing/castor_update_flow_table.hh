#ifndef CLICK_CASTOR_UPDATE_FLOW_TABLE_HH
#define CLICK_CASTOR_UPDATE_FLOW_TABLE_HH

#include <click/element.hh>
#include "../flow/castor_flow_table.hh"
#include "castor_history.hh"

CLICK_DECLS

class CastorUpdateFlowTable: public Element {
public:
	const char *class_name() const { return "CastorUpdateFlowTable"; }
	const char *port_count() const { return PORTS_1_1; }
	const char *processing() const { return AGNOSTIC; }
	int configure(Vector<String>&, ErrorHandler*);

	Packet* simple_action(Packet*);
private:
	CastorFlowTable* flowtable;
	CastorHistory* history;
};

CLICK_ENDDECLS

#endif
