#include <click/config.h>
#include <click/args.hh>
#include "castor_encrypt_ackauth.hh"
#include "castor.hh"

CLICK_DECLS

int CastorEncryptAckAuth::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_mp("Crypto", ElementCastArg("Crypto"), crypto)
			.complete();
}

Packet* CastorEncryptAckAuth::simple_action(Packet *p) {
	WritablePacket* q = p->uniqueify();
	if (!q)
		return 0;
	CastorPkt& pkt = (CastorPkt&) *q->data();

	const SymmetricKey* sk = crypto->getSharedKey(pkt.dst);
	if (!sk) {
		click_chatter("Could not find shared key for host %s. Discarding PKT...", pkt.dst.unparse().c_str());
		checked_output_push(1, q);
		return 0;
	}
	crypto->encrypt(pkt.pauth, pkt.pauth, *sk);

	return q;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorEncryptAckAuth)
