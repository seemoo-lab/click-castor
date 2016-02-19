#ifndef CLICK_CASTOR_XCAST_REMOVE_HEADER_HH
#define CLICK_CASTOR_XCAST_REMOVE_HEADER_HH

#include <click/element.hh>

CLICK_DECLS

/**
 * Removes the Xcastor header
 */
class CastorXcastRemoveHeader: public Element {
public:
	const char *class_name() const { return "CastorXcastRemoveHeader"; }
	const char *port_count() const { return "1/1"; }
	const char *processing() const { return AGNOSTIC; }

	Packet* simple_action(Packet *);
};

CLICK_ENDDECLS

#endif

