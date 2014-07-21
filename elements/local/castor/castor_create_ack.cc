#include <click/config.h>
#include <click/confparse.hh>
#include <click/packet.hh>
#include "castor_create_ack.hh"

CLICK_DECLS

CastorCreateAck::CastorCreateAck() {
}

CastorCreateAck::~CastorCreateAck() {
}

void CastorCreateAck::push(int, Packet* p) {

	// Generate new ACK
	Castor_ACK ack;
	ack.type = CastorType::MERKLE_ACK;
	ack.hsize = sizeof(Hash);
	ack.len = sizeof(Castor_ACK);
	memcpy(ack.auth, CastorPacket::getCastorAnno(p), sizeof(ACKAuth));

	// Broadcast ACK
	WritablePacket* q = Packet::make(&ack, sizeof(Castor_ACK));
	q->set_dst_ip_anno(IPAddress::make_broadcast());

	output(0).push(p); // PKT -> output 0
	output(1).push(q); // ACK -> output 1

}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorCreateAck)
