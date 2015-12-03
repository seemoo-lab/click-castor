#include <click/config.h>
#include <click/args.hh>
#include "castor_xcast_authenticate_flow.hh"
#include "castor_xcast.hh"
#include "flow/merkle_tree.hh"

CLICK_DECLS

int CastorXcastAuthenticateFlow::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_mp("Crypto", ElementCastArg("Crypto"), crypto)
			.complete();
}

Packet* CastorXcastAuthenticateFlow::simple_action(Packet *p){
	CastorXcastPkt pkt = CastorXcastPkt(p);

	// Pid is implicitly given by the PktAuth
	SValue pid = crypto->hashConvert(pkt.pkt_auth());

	SValue fid = crypto->convert(pkt.fid());

	Vector<SValue> fauth;
	fauth.reserve(pkt.flow_size());
	for(int i = 0; i < pkt.flow_size(); i++)
		fauth.push_back(crypto->convert(pkt.flow_auth()[i]));

	if(MerkleTree::isValidMerkleTree(pkt.kpkt(), pid, fauth, fid, *crypto)) {
		return pkt.getPacket();
	} else {
		checked_output_push(1, pkt.getPacket());
		return 0;
	}
}

CLICK_ENDDECLS
ELEMENT_REQUIRES(MerkleTree)
EXPORT_ELEMENT(CastorXcastAuthenticateFlow)
