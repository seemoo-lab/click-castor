#ifndef CLICK_CASTOR_XCAST_SET_HEADER_HH
#define CLICK_CASTOR_XCAST_SET_HEADER_HH

#include <click/element.hh>
#include "castor.hh"
#include "crypto/crypto.hh"
#include "castor_xcast_destination_map.hh"

CLICK_DECLS

/**
 * Sets Xcast destinations as well as individual pids
 */
class CastorXcastSetDestinations: public Element {
public:
	CastorXcastSetDestinations() : crypto(NULL), map(NULL) {}

	const char *class_name() const { return "CastorXcastSetDestinations"; }
	const char *port_count() const { return PORTS_1_1; }
	const char *processing() const { return PUSH; }
	int configure(Vector<String>&, ErrorHandler*);

	void push(int, Packet*);
private:
	Crypto* crypto;
	CastorXcastDestinationMap* map;
};

CLICK_ENDDECLS

#endif
