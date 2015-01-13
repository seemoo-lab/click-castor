#include <click/config.h>
#include <click/confparse.hh>
#include "castor_neighbors.hh"

CLICK_DECLS

CastorNeighbors::CastorNeighbors() {
	timeout = 1000;
}

CastorNeighbors::~CastorNeighbors() {
}

int CastorNeighbors::configure(Vector<String>& conf, ErrorHandler* errh) {
	return cp_va_kparse(conf, this, errh,
			"TIMEOUT", cpkP + cpkM, cpUnsigned, &timeout,
			cpEnd);
}

void CastorNeighbors::addNeighbor(Neighbor neighbor) {
	neighbors.set(neighbor, Timestamp::now());
}

bool CastorNeighbors::hasNeighbor(Neighbor neighbor) {
	Timestamp* timestamp = neighbors.get_pointer(neighbor);

	// Neighbor not found
	if(timestamp == 0)
		return false;

	// Entry for neighbor not yet outdated
	if((Timestamp::now() - *timestamp).msecval() < timeout)
		return true;

	// Entry for neighbor expired
	neighbors.erase(neighbor); // Delete entry before returning false
	return false;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorNeighbors)
