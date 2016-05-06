#ifndef CLICK_CASTOR_MIRROR_HH
#define CLICK_CASTOR_MIRROR_HH

#include <click/element.hh>
#include "../neighbor_id.hh"

CLICK_DECLS

class CastorMirror: public Element {
public:
	const char *class_name() const { return "CastorMirror"; }
	const char *port_count() const { return PORTS_1_1; }
	const char *processing() const { return AGNOSTIC; }
	int configure(Vector<String>&, ErrorHandler*);

	Packet* simple_action(Packet *);
private:
	NeighborId id;
};

CLICK_ENDDECLS

#endif
