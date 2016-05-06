#include <click/config.h>
#include <click/args.hh>
#include "add_neighbor.hh"
#include "../castor_anno.hh"

CLICK_DECLS

int AddNeighbor::configure(Vector<String>& conf, ErrorHandler* errh) {
	return Args(conf, this, errh)
			.read_mp("NEIGHBORS", ElementCastArg("Neighbors"), neighbors)
			.read_or_set_p("ENABLE", enabled, true)
			.complete();
}

Packet* AddNeighbor::simple_action(Packet* p) {
	if (enabled)
		neighbors->add(CastorAnno::src_id_anno(p));

	return p;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(AddNeighbor)
