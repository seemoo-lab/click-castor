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

	CastorPkt* pkt = (CastorPkt*) p->data();

	SValue fid(pkt->fid.data(), sizeof(FlowId));
	SValue pid(pkt->pid.data(), sizeof(PacketId));
	Vector<SValue> flow_auth;
	for (int i = 0; i < CASTOR_FLOWAUTH_ELEM; i++)
		flow_auth.push_back(SValue(pkt->fauth[i].data.data(), sizeof(Hash)));

	if (MerkleTree::isValidMerkleTree(pkt->kpkt, pid, flow_auth, fid, *crypto))
		output(0).push(p);
	else
		output(1).push(p); // Invalid -> discard

}

CLICK_ENDDECLS
ELEMENT_REQUIRES(TREE)
EXPORT_ELEMENT(CastorAuthenticateFlow)
