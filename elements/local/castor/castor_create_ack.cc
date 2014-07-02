#include <click/config.h>
#include <click/confparse.hh>
#include <click/packet.hh>
#include "castor_create_ack.hh"

CLICK_DECLS

CastorCreateACK::CastorCreateACK() {
}

CastorCreateACK::~CastorCreateACK() {
}

int CastorCreateACK::configure(Vector<String> &conf, ErrorHandler *errh) {
	return cp_va_kparse(conf, this, errh,
			"CRYPT", cpkP + cpkM, cpElementCast, "Crypto", &crypto,
			cpEnd);
}

void CastorCreateACK::push(int, Packet* p) {

	Castor_PKT* pkt = (Castor_PKT*) p->data();

	// Generate new ACK
	Castor_ACK ack;
	ack.type = CastorType::MERKLE_ACK;
	ack.hsize = sizeof(Hash);
	ack.len = sizeof(Castor_ACK);
	memcpy(ack.auth, pkt->eauth, sizeof(ACKAuth)); // eauth should be already decrypted!

	// Broadcast ACK
	WritablePacket* q = Packet::make(&ack, sizeof(Castor_ACK));
	q->set_dst_ip_anno(IPAddress::make_broadcast());

	output(0).push(p); // PKT -> output 0
	output(1).push(q); // ACK -> output 1

}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorCreateACK)
