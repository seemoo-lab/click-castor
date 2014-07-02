#include <click/config.h>
#include <click/confparse.hh>
#include "castor_validateflow.hh"
#include "tree.hh"

CLICK_DECLS

CastorValidateFlow::CastorValidateFlow(){}

CastorValidateFlow::~ CastorValidateFlow(){}

int CastorValidateFlow::configure(Vector<String> &conf, ErrorHandler *errh) {
	return cp_va_kparse(conf, this, errh,
		"CRYPT", cpkP+cpkM, cpElementCast, "Crypto", crypto,
		cpEnd);
}

void CastorValidateFlow::push(int, Packet *p){

	Castor_PKT* pkt = (Castor_PKT*) p->data();

	SValue fid(pkt->fid, sizeof(FlowId));
	SValue pid(pkt->pid, sizeof(PacketId));
	Vector<SValue> flow_auth;
	for(int i = 0; i < CASTOR_FLOWSIZE; i++)
		flow_auth.push_back(SValue(pkt->fauth[i].data, sizeof(Hash)));

	if(MerkleTree::isValidMerkleTree(pkt->packet_num, pid, flow_auth, fid, *crypto))
		output(0).push(p);
	else
		output(1).push(p); // Invalid -> discard

}

CLICK_ENDDECLS
ELEMENT_REQUIRES(TREE)
EXPORT_ELEMENT(CastorValidateFlow)
