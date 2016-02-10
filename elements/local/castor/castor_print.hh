#ifndef CLICK_CASTORPRINT_HH
#define CLICK_CASTORPRINT_HH

#include <click/element.hh>
#include "node_id.hh"

CLICK_DECLS

class CastorPrint : public Element { 
public:
	const char *class_name() const	{ return "CastorPrint"; }
	const char *port_count() const	{ return PORTS_1_1; }
	const char *processing() const	{ return AGNOSTIC; }
	int configure(Vector<String>&, ErrorHandler*);

	Packet* simple_action(Packet *);
private:
	NodeId myId;
	String label;
	bool verbose;
	bool delay;
};

CLICK_ENDDECLS

#endif
