#include <click/config.h>
#include <click/args.hh>
#include <click/confparse.hh>
#include <click/packet.hh>
#include <clicknet/ether.h>
#include "castor_xcast_create_ack.hh"
#include "castor_xcast.hh"

CLICK_DECLS

void CastorXcastCreateAck::push(int, Packet* p) {
	// Generate new ACK
	CastorXcastAck ack;
	ack.type = CastorType::XCAST_ACK;
	ack.esize = sizeof(EACKAuth);
	ack.len = sizeof(CastorXcastAck);
	memcpy(ack.auth, CastorPacket::getCastorAnno(p), sizeof(EACKAuth)); // auth should be in Castor user annotation

	// Broadcast ACK
	WritablePacket* q = Packet::make(sizeof(click_ether) + sizeof(click_ip), &ack, sizeof(CastorXcastAck), 0);
	CastorPacket::set_src_ip_anno(q, CastorXcastPkt(p).getDestination(0)); // We are source of ACK
	q->set_dst_ip_anno(CastorPacket::src_ip_anno(p)); // Set DST_ANNO to source of PKT

	output(0).push(p); // PKT -> output 0
	output(1).push(q); // ACK -> output 1

}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorXcastCreateAck)
