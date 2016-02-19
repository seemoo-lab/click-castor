#ifndef CLICK_CASTOR_XCAST_SET_HEADER_HH
#define CLICK_CASTOR_XCAST_SET_HEADER_HH

#include <click/element.hh>
#include "crypto/crypto.hh"
#include "castor_xcast_destination_map.hh"
#include "../neighbordiscovery/neighbor_id.hh"

CLICK_DECLS

/**
 * Sets Xcast destinations as well as individual pids
 */
class CastorXcastSetDestinations: public Element {
public:
	const char *class_name() const { return "CastorXcastSetDestinations"; }
	const char *port_count() const { return PORTS_1_1X2; }
	const char *processing() const { return PROCESSING_A_AH; }
	int configure(Vector<String>&, ErrorHandler*);

	Packet* simple_action(Packet*);
private:
	Crypto* crypto;
	CastorXcastDestinationMap* map;
	NeighborId my_id;
};

CLICK_ENDDECLS

#endif
