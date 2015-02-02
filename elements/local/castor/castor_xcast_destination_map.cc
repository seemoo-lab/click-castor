#include <click/config.h>
#include <click/args.hh>
#include <click/confparse.hh>

#include "castor_xcast_destination_map.hh"

CLICK_DECLS

const Vector<NodeId>& CastorXcastDestinationMap::getDestinations(GroupId multicastAddr) const {
	const Vector<NodeId>* result = _map.get_pointer(multicastAddr);
	if(result)
		return *result;
	else
		return _empty;
}

int CastorXcastDestinationMap::insertDestinations(GroupId group, const Vector<NodeId>& dests) {
	_map.set(group, dests);
	return 0;
}

int CastorXcastDestinationMap::write_handler(const String &str, Element *e, void *, ErrorHandler *errh) {
	CastorXcastDestinationMap* map = (CastorXcastDestinationMap*) e;

	NodeId group;
	Vector<NodeId> dsts;

	if(Args(map, errh).push_back_words(str)
			.read_p("GroupAddr", group)
			.read_all("IP", Args::positional, DefaultArg<NodeId>(), dsts)
			.complete() < 0)
		return -1;

	return map->insertDestinations(group, dsts);
}

void CastorXcastDestinationMap::add_handlers() {
	add_write_handler("insert", write_handler, 0);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorXcastDestinationMap)
