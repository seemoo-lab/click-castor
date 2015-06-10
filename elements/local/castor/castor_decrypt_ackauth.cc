#include <click/config.h>
#include <click/args.hh>
#include <click/confparse.hh>
#include "castor_decrypt_ackauth.hh"
#include "castor.hh"

CLICK_DECLS

int CastorDecryptAckAuth::configure(Vector<String>& conf, ErrorHandler* errh) {
	return cp_va_kparse(conf, this, errh,
		"CRYPT", cpkP+cpkM, cpElementCast, "Crypto", &crypto,
		cpEnd);
}

void CastorDecryptAckAuth::push(int, Packet *p) {

	WritablePacket* q = p->uniqueify();
	CastorPkt& pkt = (CastorPkt&) *q->data();

	SValue encAuth = crypto->convert(pkt.pauth);

	// Get appropriate key and decrypt encrypted ACK authenticator
	const SymmetricKey* sk = crypto->getSharedKey(pkt.src);
	if (!sk) {
		click_chatter("Could not find shared key for host %s. Discarding PKT...", pkt.dst.unparse().c_str());
		q->kill();
		return;
	}
	SValue auth = crypto->decrypt(encAuth, *sk);
	if (auth.size() != sizeof(PktAuth)) {
		click_chatter("Cannot create ciphertext: Crypto subsystem returned wrong plaintext length. Discarding PKT...");
		q->kill();
		return;
	}

	AckAuth& authAnno = CastorPacket::getCastorAnno(p);
	authAnno = crypto->convert(auth);

	output(0).push(q);

}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorDecryptAckAuth)

