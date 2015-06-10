#include <click/config.h>
#include <click/confparse.hh>
#include "castor_next_fid_table.hh"

CLICK_DECLS

bool CastorNextFidTable::set(const NextFlowAuth& nfauth, const FlowId& fid) {
	const FlowId* currentFid = map.get_pointer(nfauth);
	if (currentFid == 0 || *currentFid != fid) {
		map.set(nfauth, fid);
		return true;
	} else {
		return false;
	}
}

const FlowId* CastorNextFidTable::get(const NextFlowAuth& nfauth) {
	return map.get_pointer(nfauth);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorNextFidTable)
