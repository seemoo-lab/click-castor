#include <click/config.h>
#include <click/confparse.hh>
#include "castor_xcast_authenticate_flow.hh"
#include "tree.hh"
#include "castor_xcast.hh"

CLICK_DECLS

CastorXcastAuthenticateFlow::CastorXcastAuthenticateFlow(){}

CastorXcastAuthenticateFlow::~ CastorXcastAuthenticateFlow(){}

int CastorXcastAuthenticateFlow::configure(Vector<String> &conf, ErrorHandler *errh) {
	return cp_va_kparse(conf, this, errh,
			"CRYPT", cpkP + cpkM, cpElementCast, "Crypto", &crypto,
			cpEnd);
}

void CastorXcastAuthenticateFlow::push(int, Packet *p){

	CastorXcastPkt pkt = CastorXcastPkt(p);

	SValue fid(pkt.getFlowId(), sizeof(FlowId));

	// Pid is implicitly given by the AckAuth
	PacketId calcPid;
	crypto->hash(calcPid, pkt.getAckAuth(), sizeof(ACKAuth));
	SValue pid(calcPid, sizeof(PacketId));
	Vector<SValue> flow_auth;
	for(int i = 0; i < pkt.getNFlowAuthElements(); i++)
		flow_auth.push_back(SValue(pkt.getFlowAuth()[i].data, sizeof(Hash)));

	if(MerkleTree::isValidMerkleTree(pkt.getKPkt(), pid, flow_auth, fid, *crypto))
		output(0).push(pkt.getPacket());
	else
		output(1).push(pkt.getPacket()); // Invalid -> discard

}

CLICK_ENDDECLS
ELEMENT_REQUIRES(TREE)
EXPORT_ELEMENT(CastorXcastAuthenticateFlow)