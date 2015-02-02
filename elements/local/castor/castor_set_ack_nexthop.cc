#include <click/config.h>
#include <click/confparse.hh>
#include <click/packet.hh>
#include "castor_set_ack_nexthop.hh"

CLICK_DECLS

int CastorSetAckNexthop::configure(Vector<String> &conf, ErrorHandler *errh) {
    return cp_va_kparse(conf, this, errh,
		"CastorHistory", cpkP+cpkM, cpElementCast, "CastorHistory", &history,
		"Neighbors", cpkP+cpkM, cpElementCast, "Neighbors", &neighbors,
		"PROMISC", cpkP+cpkM, cpBool, &promisc,
        cpEnd);
}

void CastorSetAckNexthop::push(int, Packet* p) {
	const PacketId& pid = (PacketId&) *CastorPacket::getCastorAnno(p);
	NodeId dst = history->getPktSenders(pid)[0];  // set default ACK destination to initial PKT sender

	// Use broadcast if there are at least two other neighbors than the ACK sender
	// We also use broadcast (opportunistically) if the ACK sender is our only current neighbor
	bool use_broadcast = neighbors->neighborCount() != 2;

	// Walk through the list of PKT senders and select the first one that is still a neighbor
	for (size_t i = 0; i < history->getPkts(pid); i++) {
		NodeId pktSender = history->getPktSenders(pid)[i];
		if (neighbors->hasNeighbor(pktSender)) {
			dst = pktSender;
			break;
		}
	}

	// Set packet destination
	p->set_dst_ip_anno(use_broadcast ? NodeId::make_broadcast() : dst);
	if (promisc)
		CastorPacket::set_mac_ip_anno(p, dst);

	output(use_broadcast).push(p);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorSetAckNexthop)
