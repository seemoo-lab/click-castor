#include <click/config.h>
#include <click/args.hh>
#include <click/confparse.hh>
#include <click/packet.hh>
#include "castor_xcast_create_ack.hh"
#include "castor_xcast.hh"

CLICK_DECLS

CastorXcastCreateAck::CastorXcastCreateAck() {
}

CastorXcastCreateAck::~CastorXcastCreateAck() {
}

int CastorXcastCreateAck::configure(Vector<String> &conf, ErrorHandler *errh) {
	if(Args(conf, this, errh)
			.read_mp("ADDR", myAddr)
			.complete() < 0)
		return -1;
	return 0;
}

void CastorXcastCreateAck::push(int, Packet* p) {
	// Generate new ACK
	CastorXcastAck ack;
	ack.type = CastorType::XCAST_ACK;
	ack.esize = sizeof(EACKAuth);
	ack.len = sizeof(CastorXcastAck);
	memcpy(ack.auth, CastorPacket::getCastorAnno(p), sizeof(EACKAuth)); // auth should be in Castor user annotation

	// Broadcast ACK
	WritablePacket* q = Packet::make(&ack, sizeof(CastorXcastAck));
	q->set_dst_ip_anno(myAddr);

	output(0).push(p); // PKT -> output 0
	output(1).push(q); // ACK -> output 1

}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorXcastCreateAck)
