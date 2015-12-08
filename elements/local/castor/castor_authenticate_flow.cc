#include <click/config.h>
#include <click/confparse.hh>
#include "castor_authenticate_flow.hh"
#include "flow/merkle_tree.hh"

CLICK_DECLS

int CastorAuthenticateFlow::configure(Vector<String> &conf, ErrorHandler *errh) {
	return cp_va_kparse(conf, this, errh,
			"CRYPT", cpkP + cpkM, cpElementCast, "Crypto", &crypto,
			cpEnd);
}

void CastorAuthenticateFlow::push(int, Packet *p) {

	CastorPkt& pkt = (CastorPkt&) *p->data();

	Vector<Hash> fauth;
	fauth.reserve(pkt.fsize);
	for (int i = 0; i < pkt.fsize; i++)
		fauth.push_back(pkt.fauth[i]);

	if (MerkleTree::isValidMerkleTree(ntohs(pkt.kpkt), pkt.pid, fauth, pkt.fid, *crypto))
		output(0).push(p);
	else
		output(1).push(p); // Invalid -> discard

}

CLICK_ENDDECLS
ELEMENT_REQUIRES(MerkleTree)
EXPORT_ELEMENT(CastorAuthenticateFlow)
