#include <click/config.h>
#include <click/args.hh>
#include <click/confparse.hh>
#include "castor_encrypt_ackauth.hh"

CLICK_DECLS

CastorEncryptACKAuth::CastorEncryptACKAuth() {
	_crypto = 0;
}

int CastorEncryptACKAuth::configure(Vector<String> &conf, ErrorHandler *errh) {
	return cp_va_kparse(conf, this, errh,
			"CRYPT", cpkP + cpkM, cpElementCast, "Crypto", &_crypto,
			cpEnd);
}

void CastorEncryptACKAuth::push(int, Packet *p) {

	WritablePacket* q = p->uniqueify();
	Castor_PKT* pkt = (Castor_PKT*) q->data();
	SValue auth(pkt->eauth.data(), sizeof(ACKAuth));

	const SymmetricKey* sk = _crypto->getSharedKey(pkt->dst);
	if (!sk) {
		click_chatter("Could not find shared key for host %s. Discarding PKT...", pkt->dst.unparse().c_str());
		q->kill();
		return;
	}
	SValue cipher = _crypto->encrypt(auth, *sk);
	delete sk;
	if (cipher.size() != sizeof(EACKAuth)) {
		click_chatter("Cannot create ciphertext: Crypto subsystem returned wrong ciphertext length. Discarding PKT...");
		q->kill();
		return;
	}

	memcpy(pkt->eauth.data(), cipher.begin(), sizeof(EACKAuth));

	output(0).push(q);

}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorEncryptACKAuth)
