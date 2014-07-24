#include <click/config.h>
#include <click/args.hh>
#include <click/confparse.hh>
#include "castor_xcast_annotate_ackauth.hh"

CLICK_DECLS

CastorXcastAnnotateAckAuth::CastorXcastAnnotateAckAuth() {
}

CastorXcastAnnotateAckAuth::~CastorXcastAnnotateAckAuth() {
}

int CastorXcastAnnotateAckAuth::configure(Vector<String>& conf, ErrorHandler* errh) {
	return cp_va_kparse(conf, this, errh,
		"CRYPT", cpkP+cpkM, cpElementCast, "Crypto", &crypto,
		cpEnd);
}

void CastorXcastAnnotateAckAuth::push(int, Packet *p) {

	WritablePacket* q = p->uniqueify();
	Castor_PKT* pkt = (Castor_PKT*) q->data();

	SValue pktAuth(pkt->eauth, sizeof(EACKAuth));

	// Get appropriate key and decrypt encrypted ACK authenticator
	const SymmetricKey* sk = crypto->getSharedKey(pkt->src);
	if (!sk) {
		click_chatter("Could not find shared key for host %s. Discarding PKT...", pkt->dst.unparse().c_str());
		q->kill();
		return;
	}
	SValue ackAuth = crypto->encrypt(pktAuth, *sk);
	delete sk;
	if (ackAuth.size() != sizeof(EACKAuth)) {
		click_chatter("Cannot create ciphertext: Crypto subsystem returned wrong plaintext length. Discarding PKT...");
		q->kill();
		return;
	}

	memcpy(CastorPacket::getCastorAnno(p), ackAuth.begin(), sizeof(ACKAuth));

	output(0).push(q);

}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorXcastAnnotateAckAuth)

