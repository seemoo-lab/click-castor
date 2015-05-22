#include <click/config.h>
#include <click/args.hh>
#include <click/confparse.hh>
#include <click/packet.hh>
#include <clicknet/ether.h>
#include "castor_xcast_create_ack.hh"
#include "castor_xcast.hh"

CLICK_DECLS

void CastorXcastCreateAck::push(int, Packet* p) {

	CastorXcastAck ack;
	ack.type = CastorType::XCAST_ACK;
	ack.esize = sizeof(PktAuth);
	ack.len = sizeof(CastorXcastAck);
	ack.auth = Hash(CastorPacket::getCastorAnno(p)); // auth should be in Castor user annotation

	WritablePacket* q = Packet::make(sizeof(click_ether) + sizeof(click_ip), &ack, sizeof(CastorXcastAck), 0);
	q->set_dst_ip_anno(CastorPacket::src_ip_anno(p)); // Set DST_ANNO to source of PKT

	output(0).push(p); // PKT -> output 0
	output(1).push(q); // ACK -> output 1

}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorXcastCreateAck)
