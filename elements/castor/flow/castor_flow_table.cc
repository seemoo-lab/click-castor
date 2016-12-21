#include <click/config.h>
#include "castor_flow_table.hh"

CLICK_DECLS

CastorFlowEntry &CastorFlowTable::get(const FlowId &fid) {
	return flows[fid];
}

bool CastorFlowTable::has(const FlowId &fid) const {
	return flows.count(fid) > 0;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorFlowTable)
