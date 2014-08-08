#ifndef CLICK_CASTOR_XCAST_REMOVE_HEADER_HH
#define CLICK_CASTOR_XCAST_REMOVE_HEADER_HH

#include <click/element.hh>
#include "castor.hh"

CLICK_DECLS

/**
 * Removes the Castor header
 */
class CastorXcastRemoveHeader: public Element {
public:
	CastorXcastRemoveHeader();
	~CastorXcastRemoveHeader();
		
	const char *class_name() const { return "CastorXcastRemoveHeader"; }
	const char *port_count() const { return "1/1"; }
	const char *processing() const { return PUSH; }

	void push(int, Packet *);
};

CLICK_ENDDECLS

#endif

