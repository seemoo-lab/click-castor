#include <click/config.h>
#include <click/confparse.hh>
#include "castor_authenticate_flow.hh"
#include "tree.hh"

CLICK_DECLS

CastorAuthenticateFlow::CastorAuthenticateFlow() {
	crypto = 0;
}

int CastorAuthenticateFlow::configure(Vector<String> &conf, ErrorHandler *errh) {
	return cp_va_kparse(conf, this, errh,
			"CRYPT", cpkP + cpkM, cpElementCast, "Crypto", &crypto,
			cpEnd);
}

void CastorAuthenticateFlow::push(int, Packet *p) {

	CastorPkt& pkt = (CastorPkt&) *p->data();

	SValue fid = crypto->convert(pkt.fid);
	SValue pid = crypto->convert(pkt.pid);
	Vector<SValue> fauth;
	fauth.reserve(pkt.fsize);
	for (int i = 0; i < pkt.fsize; i++)
		fauth.push_back(crypto->convert(pkt.fauth[i]));

	if (MerkleTree::isValidMerkleTree(pkt.kpkt, pid, fauth, fid, *crypto))
		output(0).push(p);
	else
		output(1).push(p); // Invalid -> discard

}

CLICK_ENDDECLS
ELEMENT_REQUIRES(TREE)
EXPORT_ELEMENT(CastorAuthenticateFlow)
