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
	if (!flows.count(fid)) {
		flows.set(fid, new MerkleTree(fid, 1 << h, *crypto));
	}
	return flows[fid];
}

void CastorFlowTable::add(const FlowId& fid, unsigned int h, unsigned int k, const Hash& in, const Hash fauth[], unsigned int n) {
	if (!flows.count(fid)) {
		flows.set(fid, new MerkleTree(fid, 1 << h, *crypto));
	}
	MerkleTree* tree = flows[fid];
	tree->add(k, in, fauth, n);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorFlowTable)
