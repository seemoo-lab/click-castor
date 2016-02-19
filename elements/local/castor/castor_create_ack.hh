#ifndef CLICK_CASTOR_CREATE_ACK_HH
#define CLICK_CASTOR_CREATE_ACK_HH

#include <click/element.hh>
#include "flow/castor_flow_table.hh"

CLICK_DECLS

class CastorCreateAck: public Element {
public:
	const char *class_name() const { return "CastorCreateAck"; }
	const char *port_count() const { return "1/2"; }
	const char *processing() const { return PUSH; }
	int configure(Vector<String>&, ErrorHandler*);

	void push(int, Packet *);
private:
	CastorFlowTable* flowtable;
};

CLICK_ENDDECLS

#endif
