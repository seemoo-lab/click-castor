#include <click/config.h>
#include <click/args.hh>
#include <click/confparse.hh>

#include "castor_xcast_destination_map.hh"

CLICK_DECLS

CastorXcastDestinationMap::CastorXcastDestinationMap() {
	_map = HashTable<IPAddress, Vector<IPAddress> >();
	_empty = Vector<IPAddress>();

//	IPAddress baseMulti = IPAddress("224.0.2.0");
//	IPAddress baseUni = IPAddress("192.168.201.0");
//
//	for(unsigned int i = 1; i <= 5; i++) {
//		Vector<IPAddress> dsts;
//		dsts.push_back(IPAddress(baseUni.addr() + ((100 - i) << 24)));
//		_map.set(IPAddress(baseMulti.addr() + (i << 24)), dsts);
//	}

}

CastorXcastDestinationMap::~CastorXcastDestinationMap() {
}

const Vector<IPAddress>& CastorXcastDestinationMap::getDestinations(IPAddress multicastAddr) const {
	const Vector<IPAddress>* result = _map.get_pointer(multicastAddr);
	if(result)
		return *result;
	else
		return _empty;
}

int CastorXcastDestinationMap::insertDestinations(IPAddress group, const Vector<IPAddress>& dests) {
	_map.set(group, dests);
	return 0;
}

int CastorXcastDestinationMap::write_handler(const String &str, Element *e, void *, ErrorHandler *errh) {
	CastorXcastDestinationMap* map = (CastorXcastDestinationMap*) e;

	IPAddress group;
	Vector<IPAddress> dsts;

	if(Args(map, errh).push_back_words(str)
			.read_p("GroupAddr", group)
			.read_all("IP", Args::positional, DefaultArg<IPAddress>(), dsts)
			.complete() < 0)
		return -1;

	return map->insertDestinations(group, dsts);
}

void CastorXcastDestinationMap::add_handlers() {
	add_write_handler("insert", write_handler, 0);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorXcastDestinationMap)
