#include <click/config.h>
#include <click/confparse.hh>

#include "castor_xcast_destination_map.hh"

CLICK_DECLS

CastorXcastDestinationMap::CastorXcastDestinationMap() {
	_map = HashTable<IPAddress, Vector<IPAddress> >();
	_empty = Vector<IPAddress>();
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

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorXcastDestinationMap)
