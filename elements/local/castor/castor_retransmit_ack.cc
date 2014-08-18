#include <click/config.h>
#include <click/confparse.hh>
#include "castor_retransmit_ack.hh"

CLICK_DECLS

CastorRetransmitAck::CastorRetransmitAck() {
}

CastorRetransmitAck::~CastorRetransmitAck() {
}

int CastorRetransmitAck::configure(Vector<String> &conf, ErrorHandler *errh) {
     return cp_va_kparse(conf, this, errh,
        "CastorHistory", cpkP+cpkM, cpElementCast, "CastorHistory", &history,
        "ADDR", cpkP+cpkM, cpIPAddress, &myAddr,
        cpEnd);
}

void CastorRetransmitAck::push(int, Packet *p) {

	// We already forwarded a valid ACK for this PKT, so retransmit

	if (CastorPacket::isXcast(p)) {
		CastorXcastPkt pkt = CastorXcastPkt(p);

		for (uint8_t i = 0; i < pkt.getNDestinations(); i++) {
			assert(history->hasAck(pkt.getPid(i)));
			const EACKAuth& ackAuth = history->getEAckAuth(pkt.getPid(i));

			// Generate new ACK
			CastorXcastAck ack;
			ack.type = CastorType::XCAST_ACK;
			ack.esize = sizeof(EACKAuth);
			ack.len = sizeof(CastorXcastAck);
			memcpy(&ack.auth, &ackAuth, sizeof(EACKAuth));

			WritablePacket* q = Packet::make(&ack, sizeof(Castor_ACK));
			CastorPacket::set_src_ip_anno(q, myAddr);
			q->set_dst_ip_anno(CastorPacket::src_ip_anno(p)); // Unicast ACK to PKT sender

			output(0).push(q);
		}

	} else {
		Castor_PKT& pkt = (Castor_PKT&) *p->data();

		assert(history->hasAck(pkt.pid));
		const ACKAuth& ackAuth = history->getAckAuth(pkt.pid);

		// Generate new ACK
		Castor_ACK ack;
		ack.type = CastorType::MERKLE_ACK;
		ack.hsize = sizeof(Hash);
		ack.len = sizeof(Castor_ACK);
		memcpy(&ack.auth, &ackAuth, sizeof(ACKAuth));

		WritablePacket* q = Packet::make(&ack, sizeof(Castor_ACK));
		CastorPacket::set_src_ip_anno(q, myAddr);
		q->set_dst_ip_anno(CastorPacket::src_ip_anno(p)); // Unicast ACK to PKT sender
		output(0).push(q);
	}

	// No longer need PKT
	p->kill();

}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorRetransmitAck)
