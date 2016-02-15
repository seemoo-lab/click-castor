#include <click/config.h>
#include <click/args.hh>
#include "castor_authenticate_flow.hh"
#include "castor.hh"
#include "flow/merkle_tree.hh"

CLICK_DECLS

int CastorAuthenticateFlow::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_mp("Crypto", ElementCastArg("Crypto"), crypto)
			.complete();
}

Packet* CastorAuthenticateFlow::simple_action(Packet *p) {
	CastorPkt& pkt = (CastorPkt&) *p->data();
	const Hash* fauth = reinterpret_cast<const Hash*>(p->data() + sizeof(CastorPkt));

	if (MerkleTree::validate(ntohs(pkt.kpkt), pkt.pid, fauth, pkt.fsize, pkt.fid, *crypto)) {
		return p;
	} else {
		checked_output_push(1, p);
		return 0;
	}
}

CLICK_ENDDECLS
ELEMENT_REQUIRES(MerkleTree)
EXPORT_ELEMENT(CastorAuthenticateFlow)
