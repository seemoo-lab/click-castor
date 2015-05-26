#include <click/config.h>
#include <click/args.hh>
#include <click/confparse.hh>
#include "castor_encrypt_ackauth.hh"

CLICK_DECLS

int CastorEncryptAckAuth::configure(Vector<String> &conf, ErrorHandler *errh) {
	return cp_va_kparse(conf, this, errh,
			"CRYPT", cpkP + cpkM, cpElementCast, "Crypto", &crypto,
			cpEnd);
}

void CastorEncryptAckAuth::push(int, Packet *p) {

	WritablePacket* q = p->uniqueify();
	CastorPkt* pkt = (CastorPkt*) q->data();
	SValue auth(pkt->pauth.data(), sizeof(AckAuth));

	const SymmetricKey* sk = crypto->getSharedKey(pkt->dst);
	if (!sk) {
		click_chatter("Could not find shared key for host %s. Discarding PKT...", pkt->dst.unparse().c_str());
		q->kill();
		return;
	}
	SValue cipher = crypto->encrypt(auth, *sk);
	delete sk;
	if (cipher.size() != sizeof(PktAuth)) {
		click_chatter("Cannot create ciphertext: Crypto subsystem returned wrong ciphertext length. Discarding PKT...");
		q->kill();
		return;
	}

	memcpy(pkt->pauth.data(), cipher.begin(), sizeof(PktAuth));

	output(0).push(q);

}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorEncryptAckAuth)
