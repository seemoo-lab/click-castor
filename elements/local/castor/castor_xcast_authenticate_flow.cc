#include <click/config.h>
#include <click/confparse.hh>
#include "castor_xcast_authenticate_flow.hh"
#include "castor_xcast.hh"
#include "flow/merkle_tree.hh"

CLICK_DECLS

int CastorXcastAuthenticateFlow::configure(Vector<String> &conf, ErrorHandler *errh) {
	return cp_va_kparse(conf, this, errh,
			"CRYPT", cpkP + cpkM, cpElementCast, "Crypto", &crypto,
			cpEnd);
}

void CastorXcastAuthenticateFlow::push(int, Packet *p){

	CastorXcastPkt pkt = CastorXcastPkt(p);

	// Pid is implicitly given by the AckAuth
	SValue pid = crypto->hashConvert(pkt.getPktAuth());

	SValue fid = crypto->convert(pkt.getFlowId());

	Vector<SValue> fauth;
	fauth.reserve(pkt.getNFlowAuthElements());
	for(int i = 0; i < pkt.getNFlowAuthElements(); i++)
		fauth.push_back(crypto->convert(pkt.getFlowAuth()[i]));

	if(MerkleTree::isValidMerkleTree(pkt.getKPkt(), pid, fauth, fid, *crypto))
		output(0).push(pkt.getPacket());
	else
		output(1).push(pkt.getPacket()); // Invalid -> discard

}

CLICK_ENDDECLS
ELEMENT_REQUIRES(MerkleTree)
EXPORT_ELEMENT(CastorXcastAuthenticateFlow)
