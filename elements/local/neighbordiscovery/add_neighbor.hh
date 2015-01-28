#ifndef CLICK_ADD_NEIGHBOR_HH
#define CLICK_ADD_NEIGHBOR_HH

#include <click/element.hh>
#include "neighbor_beacon.hh"
#include "neighbors.hh"

CLICK_DECLS

class AddNeighbor : public Element {
public:
	AddNeighbor() : neighbors(0) {}

	const char *class_name() const { return "AddNeighbor"; }
	const char *port_count() const { return PORTS_1_1; }
	const char *processing() const { return PUSH; }
	int configure(Vector<String>&, ErrorHandler*);

	void push(int, Packet *);

private:
	Neighbors* neighbors;
};

CLICK_ENDDECLS

#endif
