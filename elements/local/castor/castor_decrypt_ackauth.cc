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
	WritablePacket* q = p->uniqueify();
	CastorPkt& pkt = (CastorPkt&) *q->data();

	SValue encAuth = crypto->convert(pkt.pauth);

	// Get appropriate key and decrypt encrypted ACK authenticator
	const SymmetricKey* sk = crypto->getSharedKey(pkt.src);
	if (!sk) {
		click_chatter("Could not find shared key for host %s. Discarding PKT...", pkt.dst.unparse().c_str());
		checked_output_push(1, q);
		return;
	}
	SValue auth = crypto->decrypt(encAuth, *sk);
	if (auth.size() != sizeof(PktAuth)) {
		click_chatter("Cannot create ciphertext: Crypto subsystem returned wrong plaintext length. Discarding PKT...");
		checked_output_push(1, q);
		return;
	}

	AckAuth& authAnno = CastorAnno::hash_anno(p);
	authAnno = crypto->convert(auth);

	output(0).push(q);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorDecryptAckAuth)

