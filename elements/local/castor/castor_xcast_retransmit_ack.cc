#include <click/config.h>
#include <click/confparse.hh>
#include <clicknet/ether.h>
#include "castor_xcast_retransmit_ack.hh"
#include "castor_xcast.hh"

CLICK_DECLS

int CastorXcastRetransmitAck::configure(Vector<String> &conf, ErrorHandler *errh) {
     return cp_va_kparse(conf, this, errh,
        "CastorHistory", cpkP+cpkM, cpElementCast, "CastorHistory", &history,
        "ADDR", cpkP+cpkM, cpIPAddress, &myId,
        cpEnd);
}

void CastorXcastRetransmitAck::push(int, Packet *p) {
	// We already forwarded a valid ACK for this PKT, so retransmit
	CastorXcastPkt pkt = CastorXcastPkt(p);

	p = pkt.getPacket();

	for (uint8_t i = 0; i < pkt.getNDestinations(); i++) {
		assert(history->hasAck(pkt.getPid(i)));

		// Generate new ACK
		CastorAck ack;
		ack.type = CastorType::MERKLE_ACK;
		ack.hsize = sizeof(PktAuth);
		ack.len = sizeof(CastorAck);
		ack.auth = history->getAckAuth(pkt.getPid(i));
#ifdef DEBUG_ACK_SRCDST
		ack.src = pkt.getSource();
		ack.dst = pkt.getDestination(i);
#endif

		WritablePacket* q = Packet::make(sizeof(click_ether) + sizeof(click_ip), &ack, sizeof(CastorAck), 0);
		CastorPacket::dst_id_anno(q) = CastorPacket::src_id_anno(p); // Unicast ACK to PKT sender

		assert(history->hasPktFrom(pkt.getPid(i), CastorPacket::dst_id_anno(q)));

		output(0).push(q);
	}

	// No longer need PKT
	p->kill();
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorXcastRetransmitAck)
