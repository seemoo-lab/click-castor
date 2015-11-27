#ifndef CLICK_ADD_NEIGHBOR_HH
#define CLICK_ADD_NEIGHBOR_HH

#include <click/element.hh>
#include "neighbors.hh"

CLICK_DECLS

class AddNeighbor : public Element {
public:
	const char *class_name() const { return "AddNeighbor"; }
	const char *port_count() const { return "1/1"; }
	const char *processing() const { return AGNOSTIC; }
	int configure(Vector<String>&, ErrorHandler*);

	Packet* simple_action(Packet *);
private:
	Neighbors* neighbors;
	bool enabled;
};

CLICK_ENDDECLS

#endif
