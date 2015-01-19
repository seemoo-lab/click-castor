#include <click/config.h>
#include <click/confparse.hh>
#include "castor_xcast_authenticate_flow.hh"
#include "tree.hh"
#include "castor_xcast.hh"

CLICK_DECLS

CastorXcastAuthenticateFlow::CastorXcastAuthenticateFlow() {
	crypto = 0;
}

int CastorXcastAuthenticateFlow::configure(Vector<String> &conf, ErrorHandler *errh) {
	return cp_va_kparse(conf, this, errh,
			"CRYPT", cpkP + cpkM, cpElementCast, "Crypto", &crypto,
			cpEnd);
}

void CastorXcastAuthenticateFlow::push(int, Packet *p){

	CastorXcastPkt pkt = CastorXcastPkt(p);

	SValue fid(&pkt.getFlowId()[0], sizeof(FlowId));

	// Pid is implicitly given by the AckAuth
	SValue pid = crypto->hash(SValue(&pkt.getAckAuth()[0], sizeof(ACKAuth)));
	Vector<SValue> flow_auth;
	for(int i = 0; i < pkt.getNFlowAuthElements(); i++)
		flow_auth.push_back(SValue(&pkt.getFlowAuth()[i].data[0], sizeof(Hash)));

	if(MerkleTree::isValidMerkleTree(pkt.getKPkt(), pid, flow_auth, fid, *crypto))
		output(0).push(pkt.getPacket());
	else
		output(1).push(pkt.getPacket()); // Invalid -> discard

}

CLICK_ENDDECLS
ELEMENT_REQUIRES(TREE)
EXPORT_ELEMENT(CastorXcastAuthenticateFlow)
