#include <click/config.h>
#include <click/args.hh>
#include "castor_decrypt_ackauth.hh"
#include "castor.hh"
#include "castor_anno.hh"

CLICK_DECLS

int CastorDecryptAckAuth::configure(Vector<String>& conf, ErrorHandler* errh) {
	return Args(conf, this, errh)
			.read_mp("Crypto", ElementCastArg("Crypto"), crypto)
			.complete();
}

Packet* CastorDecryptAckAuth::simple_action(Packet *p) {
	CastorPkt& pkt = (CastorPkt&) *p->data();

	// Get appropriate key and decrypt encrypted ACK authenticator
	const SymmetricKey* sk = crypto->getSharedKey(pkt.src);
	if (!sk) {
		click_chatter("Could not find shared key for host %s. Discarding PKT...", pkt.dst.unparse().c_str());
		checked_output_push(1, p);
		return 0;
	}
	uint8_t insecure[Crypto::nonce_size] = {}; // TODO: should use a secure nonce, e.g., fid + pid
	crypto->stream_xor(CastorAnno::hash_anno(p), pkt.pauth, insecure, *sk);

	return p;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorDecryptAckAuth)

