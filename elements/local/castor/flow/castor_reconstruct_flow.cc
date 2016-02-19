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

	unsigned int size = 1 << pkt.fsize;
	e.aauths = new Hash[size];
	e.pids =   new Hash[size];
	// FIXME use actual end-to-end key
	Buffer<32> key;
	// Generate aauths from n
	crypto->stream(e.aauths->data(), size * pkt.hsize, pkt.n()->data(), key.data());
	for (int i = 0; i < size; i++) {
		crypto->hash(e.pids[i], e.aauths[i]);
	}
	e.tree = new MerkleTree(e.pids, size, *crypto);

	return p;
}

CLICK_ENDDECLS
ELEMENT_REQUIRES(MerkleTree)
EXPORT_ELEMENT(CastorReconstructFlow)
