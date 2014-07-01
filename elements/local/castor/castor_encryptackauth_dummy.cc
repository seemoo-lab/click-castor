#include <click/config.h>
#include <click/args.hh>
#include <click/confparse.hh>
#include "castor_encryptackauth_dummy.hh"

CLICK_DECLS

CastorEncryptACKAuthDummy::CastorEncryptACKAuthDummy() {
}

CastorEncryptACKAuthDummy::~CastorEncryptACKAuthDummy() {
}

int CastorEncryptACKAuthDummy::configure(Vector<String> &conf, ErrorHandler *errh) {
	return cp_va_kparse(conf, this, errh,
		"CRYPT", cpkP+cpkM, cpElementCast, "Crypto", &_crypto,
		cpEnd);
}

void CastorEncryptACKAuthDummy::push(int, Packet *p) {

	WritablePacket* q = p->uniqueify();
	Castor_PKT* header = (Castor_PKT*) q->data();
	SValue auth(header->eauth, CASTOR_HASHLENGTH);

	_crypto->testSymmetricCrypt(auth, header->dst);

	output(0).push(p);

}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorEncryptACKAuthDummy)
