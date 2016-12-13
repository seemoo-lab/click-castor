#ifndef CLICK_CASTOR_DEBUG_PRINT_HH
#define CLICK_CASTOR_DEBUG_PRINT_HH

#include <click/element.hh>
#include "../castor.hh"

CLICK_DECLS

class CastorDebugPrint : public Element {
public:
	const char *class_name() const { return "CastorDebugPrint"; }
	const char *port_count() const { return PORTS_1_1; }
	const char *processing() const { return AGNOSTIC; }
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
