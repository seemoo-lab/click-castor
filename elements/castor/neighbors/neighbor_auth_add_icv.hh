#ifndef CLICK_NEIGHBOR_AUTH_ADD_ICV_HH
#define CLICK_NEIGHBOR_AUTH_ADD_ICV_HH

#include <click/element.hh>
#include "neighbors.hh"
#include "../crypto/crypto.hh"

CLICK_DECLS

class NeighborAuthAddICV : public Element {
public:
	const char *class_name() const { return "NeighborAuthAddICV"; }
	const char *port_count() const { return PORTS_1_1; }
	const char *processing() const { return AGNOSTIC; }
	int configure(Vector<String>&, ErrorHandler*);

	Packet* simple_action(Packet *);
private:
	Neighbors* neighbors;
	Crypto* crypto;
	bool enable;
};

CLICK_ENDDECLS

#endif
