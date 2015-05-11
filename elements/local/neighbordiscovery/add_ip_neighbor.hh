#ifndef CLICK_ADD_IP_NEIGHBOR_HH
#define CLICK_ADD_IP_NEIGHBOR_HH

#include <click/element.hh>
#include "neighbor_beacon.hh"
#include "neighbors.hh"

CLICK_DECLS

class AddIPNeighbor : public Element {
public:
	AddIPNeighbor() : neighbors(0), enabled(true) {}

	const char *class_name() const { return "AddIPNeighbor"; }
	const char *port_count() const { return PORTS_1_1; }
	const char *processing() const { return PUSH; }
	int configure(Vector<String>&, ErrorHandler*);

	void push(int, Packet *);

private:
	Neighbors* neighbors;
	bool enabled;
};

CLICK_ENDDECLS

#endif
