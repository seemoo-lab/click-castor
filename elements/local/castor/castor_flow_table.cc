#include <click/config.h>
#include <click/args.hh>
#include "castor_flow_table.hh"

CLICK_DECLS

int CastorFlowTable::configure(Vector<String> &conf, ErrorHandler *errh) {
    return Args(conf, this, errh)
    		.read_mp("Crypto", ElementCastArg("Crypto"), crypto)
			.complete();
}

MerkleTree* CastorFlowTable::get(const FlowId& fid, unsigned int h) {
	if (flows.count(fid) == 0) {
		flows.set(fid, new MerkleTree(fid, 1 << h, *crypto));
	}
	return flows[fid];
}

NeighborId& CastorFlowTable::last(const FlowId& fid) {
	return last_neighbor[fid];
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorFlowTable)
