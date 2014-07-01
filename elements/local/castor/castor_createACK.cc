#include <click/config.h>
#include <click/confparse.hh>
#include <click/packet.hh>
#include "castor_createACK.hh"

CLICK_DECLS

CastorCreateACK::CastorCreateACK() {
}

CastorCreateACK::~CastorCreateACK() {
}

int CastorCreateACK::configure(Vector<String> &conf, ErrorHandler *errh) {
	if(cp_va_kparse(conf, this, errh,
			"CRYPT", cpkP + cpkM, cpElementCast, "Crypto", &_crypto,
			cpEnd)
			< 0)
		return -1;
	return 0;
}

/**
 * Create an ACK for incoming packet. The original packet is pushed to output 0, the ACK is pushed on output 1
 */
void CastorCreateACK::push(int, Packet* p) {

	Castor_PKT* pkt = (Castor_PKT*) p->data();

	// Decrypt the authenticator
	SValue eauth = SValue(pkt->eauth, CASTOR_ENCLENGTH);
	SymmetricKey* sk = _crypto->getSharedKey(pkt->src);
	if (!sk) {
		click_chatter("Could not find shared key for host %s. Cannot create ACK. Discarding PKT...", pkt->src.unparse().c_str());
		p->kill();
		return;
	}
	SValue aauth = _crypto->decrypt(eauth, *sk);

	// Generate new ACK
	Castor_ACK ack;
	ack.type = CASTOR_TYPE_ACK;
	ack.hsize = CASTOR_HASHLENGTH;
	ack.len = sizeof(Castor_ACK);
	memcpy(&ack.auth, aauth.begin(), CASTOR_HASHLENGTH);

	// Broadcast ACK
	WritablePacket* q = Packet::make(&ack, sizeof(Castor_ACK));
	q->set_dst_ip_anno(IPAddress::make_broadcast());

	output(0).push(p); // PKT -> output 0
	output(1).push(q); // ACK -> output 1

}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorCreateACK)
