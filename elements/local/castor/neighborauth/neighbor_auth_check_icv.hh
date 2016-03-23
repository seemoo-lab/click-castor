#ifndef CLICK_NEIGHBOR_AUTH_CHECK_ICV_HH
#define CLICK_NEIGHBOR_AUTH_CHECK_ICV_HH

#include <click/element.hh>
#include "../crypto/crypto.hh"

CLICK_DECLS

class NeighborAuthCheckICV : public Element {
public:
	const char *class_name() const { return "NeighborAuthCheckICV"; }
	const char *port_count() const { return PORTS_1_1X2; }
	const char *processing() const { return PROCESSING_A_AH; }
	int configure(Vector<String>&, ErrorHandler*);

	Packet* simple_action(Packet *);
private:
	Crypto* crypto;
};

CLICK_ENDDECLS

#endif
