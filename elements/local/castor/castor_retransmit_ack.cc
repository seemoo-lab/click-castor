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
	if (CastorPacket::isXcast(p)) {
		CastorXcastPkt pkt = CastorXcastPkt(p);

		p = pkt.getPacket();

		for (uint8_t i = 0; i < pkt.getNDestinations(); i++) {
			assert(history->hasAck(pkt.getPid(i)));

			// Generate new ACK
			CastorXcastAck ack;
			ack.type = CastorType::XCAST_ACK;
			ack.esize = sizeof(PktAuth);
			ack.len = sizeof(CastorXcastAck);
			ack.auth = history->getEAckAuth(pkt.getPid(i));

			WritablePacket* q = Packet::make(sizeof(click_ether) + sizeof(click_ip), &ack, sizeof(CastorXcastAck), 0);
			q->set_dst_ip_anno(CastorPacket::src_ip_anno(p)); // Unicast ACK to PKT sender

			assert(history->hasPktFrom(pkt.getPid(i), q->dst_ip_anno()));

			output(0).push(q);

		}

	} else {
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
		q->set_dst_ip_anno(CastorPacket::src_ip_anno(p)); // Unicast ACK to PKT sender

		assert(history->hasPktFrom(pkt.pid, q->dst_ip_anno()));

		output(0).push(q);
	}

	// No longer need PKT
	p->kill();
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorRetransmitAck)
