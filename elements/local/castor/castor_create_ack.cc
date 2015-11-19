#include <click/config.h>
#include <click/confparse.hh>
#include <click/packet.hh>
#include <clicknet/ether.h>
#include "castor_create_ack.hh"

CLICK_DECLS

void CastorCreateAck::push(int, Packet* p) {
	CastorAck ack;
	ack.type = CastorType::MERKLE_ACK;
	ack.hsize = sizeof(AckAuth);
	ack.len = sizeof(CastorAck);
	ack.auth = CastorPacket::getCastorAnno(p);

	// Make enough room for prepended headers
	WritablePacket* q = Packet::make(sizeof(click_ether) + sizeof(click_ip), &ack, sizeof(CastorAck), 0);
	CastorPacket::dst_id_anno(q) = CastorPacket::src_id_anno(p); // Set DST_ANNO to source of PKT

	output(0).push(p); // PKT -> output 0
	output(1).push(q); // ACK -> output 1
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorCreateAck)
