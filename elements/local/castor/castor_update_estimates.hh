#ifndef CLICK_CASTOR_UPDATE_ESTIMATES_HH
#define CLICK_CASTOR_UPDATE_ESTIMATES_HH

#include <click/element.hh>
#include "castor_history.hh"
#include "castor_routing_table.hh"

CLICK_DECLS

class CastorUpdateEstimates: public Element {
public:
	inline CastorUpdateEstimates() : table(NULL), history(NULL) {}

	const char *class_name() const { return "CastorUpdateEstimates"; }
	const char *port_count() const { return "1/2"; }
	const char *processing() const { return PUSH; }
	int configure(Vector<String>&, ErrorHandler*);

	void push(int, Packet *);
private:
	CastorRoutingTable* table;
	CastorHistory* history;
};

CLICK_ENDDECLS

#endif
