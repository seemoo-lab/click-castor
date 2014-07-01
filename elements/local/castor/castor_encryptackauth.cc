#include <click/config.h>
#include <click/args.hh>
#include <click/confparse.hh>
#include "castor_encryptackauth.hh"

CLICK_DECLS

CastorEncryptACKAuth::CastorEncryptACKAuth() {
}

CastorEncryptACKAuth::~CastorEncryptACKAuth() {
}

int CastorEncryptACKAuth::configure(Vector<String> &conf, ErrorHandler *errh) {
	if (cp_va_kparse(conf, this, errh,
			"CRYPT", cpkP + cpkM, cpElementCast, "Crypto", &_crypto,
			cpEnd)
			< 0)
		return -1;
	return 0;
}

void CastorEncryptACKAuth::push(int, Packet *p) {

	WritablePacket* q = p->uniqueify();
	Castor_PKT* pkt = (Castor_PKT*) q->data();
	SValue auth(pkt->eauth, CASTOR_HASHLENGTH);

	SymmetricKey* sk = _crypto->getSharedKey(pkt->dst);
	if (!sk) {
		click_chatter("Could not find shared key for host %s. Discarding PKT...", pkt->dst.unparse().c_str());
		q->kill();
		return;
	}
	SValue cipher = _crypto->encrypt(auth, *sk);
	if (cipher.size() != CASTOR_ENCLENGTH) {
		click_chatter("Cannot create ciphertext: Crypto subsystem returned wrong ciphertext length. Discarding PKT...");
		q->kill();
		return;
	}

	memcpy(pkt->eauth, cipher.begin(), CASTOR_ENCLENGTH);

	output(0).push(q);

}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorEncryptACKAuth)

