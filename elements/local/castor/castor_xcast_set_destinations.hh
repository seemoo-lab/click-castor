#ifndef CLICK_CASTOR_XCAST_SET_HEADER_HH
#define CLICK_CASTOR_XCAST_SET_HEADER_HH

#include <click/element.hh>
#include "castor.hh"
#include "crypto.hh"
#include "castor_xcast_destination_map.hh"

CLICK_DECLS

/**
 * Sets Xcast destinations as well as individual pids
 */
class CastorXcastSetDestinations: public Element {
public:
	CastorXcastSetDestinations();
	~CastorXcastSetDestinations();
		
	const char *class_name() const { return "CastorXcastSetDestinations"; }
	const char *port_count() const { return "1/1"; }
	const char *processing() const { return PUSH; }
	int configure(Vector<String>&, ErrorHandler*);

	void push(int, Packet*);
private:
	Crypto* _crypto;
	CastorXcastDestinationMap* _map;
};

CLICK_ENDDECLS

#endif
