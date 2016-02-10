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
	Hash pid;
	crypto->hash(pid, pkt.pkt_auth());

	if(MerkleTree::validate(ntohs(pkt.kpkt()), pid, pkt.flow_auth().elem, pkt.flow_size(), pkt.fid(), *crypto)) {
		return pkt.getPacket();
	} else {
		checked_output_push(1, pkt.getPacket());
		return 0;
	}
}

CLICK_ENDDECLS
ELEMENT_REQUIRES(MerkleTree)
EXPORT_ELEMENT(CastorXcastAuthenticateFlow)
