#include <click/config.h>
#include <click/args.hh>
#include "castor_authenticate_flow.hh"
#include "../castor.hh"
#include "merkle_tree.hh"

CLICK_DECLS

int CastorAuthenticateFlow::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_mp("Flows", ElementCastArg("CastorFlowTable"), flowtable)
			.read_mp("Crypto", ElementCastArg("Crypto"), crypto)
			.complete();
}

Packet* CastorAuthenticateFlow::simple_action(Packet *p) {
	CastorPkt& pkt = (CastorPkt&) *p->data();
	CastorFlowEntry& e = flowtable->get(pkt.fid);
	if (e.tree == NULL)
		e.tree = new MerkleTree(pkt.fid, 1 << pkt.fsize, *crypto);

	int result = e.tree->valid_leaf(ntohs(pkt.kpkt), pkt.pid, pkt.fauth(), pkt.fasize);
	if (result == 0) {
		// TODO add valid leafs and intermediate nodes to the tree while checking. flowtable->add(.) will compute all hashes again
		e.tree->add(ntohs(pkt.kpkt), pkt.pid, pkt.fauth(), pkt.fasize);
		return p;
	} else {
		checked_output_push(-result, p); // 1 or 2
		return 0;
	}
}

CLICK_ENDDECLS
ELEMENT_REQUIRES(MerkleTree)
EXPORT_ELEMENT(CastorAuthenticateFlow)
