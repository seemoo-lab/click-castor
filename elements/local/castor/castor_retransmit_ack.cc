#include <click/config.h>
#include <click/confparse.hh>
#include <clicknet/ether.h>
#include "castor_retransmit_ack.hh"

CLICK_DECLS

int CastorRetransmitAck::configure(Vector<String> &conf, ErrorHandler *errh) {
     return cp_va_kparse(conf, this, errh,
        "CastorHistory", cpkP+cpkM, cpElementCast, "CastorHistory", &history,
        "ADDR", cpkP+cpkM, cpIPAddress, &myId,
        cpEnd);
}

void CastorRetransmitAck::push(int, Packet *p) {
	// We already forwarded a valid ACK for this PKT, so retransmit
	CastorPkt& pkt = (CastorPkt&) *p->data();

	assert(history->hasAck(pkt.pid));

	// Generate new ACK
	CastorAck ack;
	ack.type = CastorType::MERKLE_ACK;
	ack.hsize = sizeof(Hash);
	ack.len = sizeof(CastorAck);
	ack.auth = history->getAckAuth(pkt.pid);
#ifdef DEBUG_ACK_SRCDST
	ack.src = pkt.dst;
	ack.dst = pkt.src;
#endif

	WritablePacket* q = Packet::make(sizeof(click_ether) + sizeof(click_ip), &ack, sizeof(CastorAck), 0);
	CastorPacket::dst_id_anno(q) = CastorPacket::src_id_anno(p); // Unicast ACK to PKT sender

	assert(history->hasPktFrom(pkt.pid, CastorPacket::dst_id_anno(q)));

	// No longer need PKT
	p->kill();

	output(0).push(q);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorRetransmitAck)
