#include <click/config.h>
#include <click/args.hh>
#include "castor_flow_table.hh"

CLICK_DECLS

int CastorFlowTable::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_mp("Crypto", ElementCastArg("Crypto"), crypto)
			.complete();
}

CastorFlowEntry& CastorFlowTable::get(const FlowId& fid) {
	return flows[fid];
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorFlowTable)
