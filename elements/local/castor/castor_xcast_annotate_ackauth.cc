#include <click/config.h>
#include <click/args.hh>
#include "castor_xcast_annotate_ackauth.hh"
#include "castor_xcast.hh"
#include "castor_anno.hh"

CLICK_DECLS

int CastorXcastAnnotateAckAuth::configure(Vector<String>& conf, ErrorHandler* errh) {
	return Args(conf, this, errh)
			.read_mp("Crypto", ElementCastArg("Crypto"), crypto)
			.complete();
}

Packet* CastorXcastAnnotateAckAuth::simple_action(Packet *p) {
	CastorXcastPkt pkt = CastorXcastPkt(p);

	// Get appropriate key and decrypt encrypted ACK authenticator
	const SymmetricKey* sk = crypto->getSharedKey(pkt.src());
	if (!sk) {
		click_chatter("Could not find shared key for host %s. Discarding PKT...", pkt.dst(0).unparse().c_str());
		checked_output_push(1, pkt.getPacket());
		return 0;
	}
	uint8_t insecure[Crypto::nonce_size] = {}; // TODO: should use a secure nonce, e.g., fid + pid
	crypto->stream_xor(CastorAnno::hash_anno(pkt.getPacket()), pkt.pkt_auth(), insecure, *sk);

	return pkt.getPacket();
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorXcastAnnotateAckAuth)

