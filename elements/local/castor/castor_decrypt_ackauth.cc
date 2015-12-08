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

void CastorDecryptAckAuth::push(int, Packet *p) {
	CastorPkt& pkt = (CastorPkt&) *p->data();

	// Get appropriate key and decrypt encrypted ACK authenticator
	const SymmetricKey* sk = crypto->getSharedKey(pkt.src);
	if (!sk) {
		click_chatter("Could not find shared key for host %s. Discarding PKT...", pkt.dst.unparse().c_str());
		checked_output_push(1, p);
		return;
	}
	CastorAnno::hash_anno(p) = crypto->decrypt(pkt.pauth, *sk);

	output(0).push(p);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorDecryptAckAuth)

