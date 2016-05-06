#ifndef CLICK_REMOVE_FORWARDER_LIST_HH
#define CLICK_REMOVE_FORWARDER_LIST_HH

#include <click/element.hh>

CLICK_DECLS

class RemoveForwarderList: public Element {
public:
	const char *class_name() const { return "RemoveForwarderList"; }
	const char *port_count() const { return PORTS_1_1; }
	const char *processing() const { return AGNOSTIC; }

	Packet* simple_action(Packet*);
};

CLICK_ENDDECLS

#endif
