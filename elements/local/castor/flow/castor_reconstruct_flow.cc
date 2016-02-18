#include <click/config.h>
#include <click/args.hh>
#include "castor_reconstruct_flow.hh"
#include "../castor.hh"
#include "../castor_anno.hh"

CLICK_DECLS

int CastorReconstructFlow::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_mp("Flows", ElementCastArg("CastorFlowTable"), flowtable)
			.read_mp("Crypto", ElementCastArg("Crypto"), crypto)
			.complete();
}

Packet* CastorReconstructFlow::simple_action(Packet *p) {
	const CastorPkt& pkt = *reinterpret_cast<const CastorPkt*>(p->data());

	assert(pkt.syn);

	CastorFlowEntry& e = flowtable->get(pkt.fid);

	if (e.complete()) {
		return p;
	}

	e.height = pkt.fsize;
	e.aauths = new Hash[e.size()];
	e.pids =   new Hash[e.size()];
	// FIXME use actual end-to-end key
	Buffer<32> key;
	// Generate aauths from n
	crypto->stream(e.aauths->data(), e.size() * sizeof(Hash), pkt.n()->data(), key.data());
	for (int i = 0; i < e.size(); i++) {
		crypto->hash(e.pids[i], e.aauths[i]);
	}
	e.tree = new MerkleTree(e.pids, e.size(), *crypto);

	return p;
}

CLICK_ENDDECLS
ELEMENT_REQUIRES(MerkleTree)
EXPORT_ELEMENT(CastorReconstructFlow)
