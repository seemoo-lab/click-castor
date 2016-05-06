#ifndef CLICK_NEIGHBOR_AUTH_STRIP_ICV_HH
#define CLICK_NEIGHBOR_AUTH_STRIP_ICV_HH

#include <click/element.hh>

CLICK_DECLS

class NeighborAuthStripICV: public Element {
public:
	const char *class_name() const { return "NeighborAuthStripICV"; }
	const char *port_count() const { return PORTS_1_1; }
	const char *processing() const { return AGNOSTIC; }

	Packet* simple_action(Packet *);
};

CLICK_ENDDECLS

#endif

