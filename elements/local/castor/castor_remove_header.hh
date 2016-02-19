#ifndef CLICK_CASTOR_REMOVE_HEADER_HH
#define CLICK_CASTOR_REMOVE_HEADER_HH

#include <click/element.hh>

CLICK_DECLS

/**
 * Removes the Castor header
 */
class CastorRemoveHeader: public Element {
public:
	const char *class_name() const { return "CastorRemoveHeader"; }
	const char *port_count() const { return PORTS_1_1; }
	const char *processing() const { return AGNOSTIC; }

	Packet* simple_action(Packet *);
};

CLICK_ENDDECLS

#endif

