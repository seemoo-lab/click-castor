#ifndef CLICK_CASTOR_ANNOTATE_ID_HH
#define CLICK_CASTOR_ANNOTATE_ID_HH

#include <click/element.hh>
#include "castor.hh"
#include "../neighbordiscovery/neighbor_id.hh"

CLICK_DECLS

class CastorAnnotateId: public Element {
public:
	const char *class_name() const { return "CastorAnnotateId"; }
	const char *port_count() const { return "1/1"; }
	const char *processing() const { return AGNOSTIC; }

	int configure(Vector<String>&, ErrorHandler*);

	Packet* simple_action(Packet *);
private:
	NeighborId id;
	uint32_t offset;
};

CLICK_ENDDECLS

#endif
