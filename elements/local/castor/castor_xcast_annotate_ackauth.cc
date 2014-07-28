#include <click/config.h>
#include <click/args.hh>
#include <click/confparse.hh>
#include "castor_xcast_annotate_ackauth.hh"
#include "castor_xcast.hh"

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
	CastorXcastPkt pkt = CastorXcastPkt(p);

	SValue pktAuth(pkt.getAckAuth(), sizeof(ACKAuth));

	// Get appropriate key and decrypt encrypted ACK authenticator
	const SymmetricKey* sk = crypto->getSharedKey(pkt.getSource());
	if (!sk) {
		click_chatter("Could not find shared key for host %s. Discarding PKT...", pkt.getDestination(0).unparse().c_str());
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

	memcpy(CastorPacket::getCastorAnno(p), ackAuth.begin(), sizeof(EACKAuth));

	output(0).push(q);

}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorXcastAnnotateAckAuth)

