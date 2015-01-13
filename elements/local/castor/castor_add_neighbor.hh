#ifndef CLICK_CASTOR_ADD_NEIGHBOR_HH
#define CLICK_CASTOR_ADD_NEIGHBOR_HH

#include <click/element.hh>
#include "castor.hh"
#include "castor_neighbors.hh"

CLICK_DECLS

class CastorAddNeighbor : public Element {
public:
	CastorAddNeighbor();
	~CastorAddNeighbor();

	const char *class_name() const { return "CastorAddNeighbor"; }
	const char *port_count() const { return PORTS_1_1; }
	const char *processing() const { return PUSH; }
	int configure(Vector<String>&, ErrorHandler*);

	void push(int, Packet *);

private:
	CastorNeighbors* neighbors;
};

CLICK_ENDDECLS

#endif
