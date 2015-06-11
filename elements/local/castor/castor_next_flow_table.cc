#include <click/config.h>
#include <click/confparse.hh>
#include "castor_next_flow_table.hh"

CLICK_DECLS

bool CastorNextFlowTable::set(const NextFlowAuth& nfauth, const FlowId& fid) {
	const FlowId* currentFid = map.get_pointer(nfauth);
	if (currentFid == 0 || *currentFid != fid) {
		map.set(nfauth, fid);
		return true;
	} else {
		return false;
	}
}

bool CastorNextFlowTable::remove(const NextFlowAuth& nfauth) {
	return map.erase(nfauth);
}

const FlowId* CastorNextFlowTable::get(const NextFlowAuth& nfauth) const {
	return map.get_pointer(nfauth);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorNextFlowTable)
