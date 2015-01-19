#include <click/config.h>
#include <click/args.hh>
#include <click/confparse.hh>
#include "castor_xcast_annotate_ackauth.hh"
#include "castor_xcast.hh"

CLICK_DECLS

CastorXcastAnnotateAckAuth::CastorXcastAnnotateAckAuth() {
	crypto = 0;
}

int CastorXcastAnnotateAckAuth::configure(Vector<String>& conf, ErrorHandler* errh) {
	return cp_va_kparse(conf, this, errh,
		"CRYPT", cpkP+cpkM, cpElementCast, "Crypto", &crypto,
		cpEnd);
}

void CastorXcastAnnotateAckAuth::push(int, Packet *p) {

	CastorXcastPkt pkt = CastorXcastPkt(p->clone());

	SValue pktAuth(pkt.getAckAuth().data(), sizeof(ACKAuth));

	// Get appropriate key and decrypt encrypted ACK authenticator
	const SymmetricKey* sk = crypto->getSharedKey(pkt.getSource());
	if (!sk) {
		click_chatter("Could not find shared key for host %s. Discarding PKT...", pkt.getDestination(0).unparse().c_str());
		pkt.getPacket()->kill();
		return;
	}
	SValue ackAuth = crypto->encrypt(pktAuth, *sk);
	delete sk;
	if (ackAuth.size() != sizeof(EACKAuth)) {
		click_chatter("Cannot create ciphertext: Crypto subsystem returned wrong plaintext length. Discarding PKT...");
		pkt.getPacket()->kill();
		return;
	}

	memcpy(CastorPacket::getCastorAnno(pkt.getPacket()), ackAuth.begin(), sizeof(EACKAuth));

	output(0).push(pkt.getPacket());

}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorXcastAnnotateAckAuth)

