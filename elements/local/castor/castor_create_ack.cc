#include <click/config.h>
#include "castor_create_ack.hh"
#include "castor.hh"
#include "castor_anno.hh"

CLICK_DECLS

void CastorCreateAck::push(int, Packet* p) {
	CastorAck ack;
	ack.type = CastorType::MERKLE_ACK;
	ack.hsize = sizeof(AckAuth);
	ack.len = sizeof(CastorAck);
	ack.auth = CastorAnno::hash_anno(p);

	WritablePacket* q = Packet::make(&ack, sizeof(CastorAck));
	CastorAnno::dst_id_anno(q) = CastorAnno::src_id_anno(p); // Set DST_ANNO to source of PKT
	CastorAnno::hop_id_anno(q) = CastorAnno::dst_id_anno(q);

	output(0).push(p); // PKT -> output 0
	output(1).push(q); // ACK -> output 1
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorCreateAck)
