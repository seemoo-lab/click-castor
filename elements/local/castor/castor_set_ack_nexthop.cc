#include <click/config.h>
#include <click/confparse.hh>
#include <click/packet.hh>
#include <click/ipaddress.hh>
#include "castor_set_ack_nexthop.hh"

CLICK_DECLS

int CastorSetAckNexthop::configure(Vector<String> &conf, ErrorHandler *errh) {
    return cp_va_kparse(conf, this, errh,
		"CastorHistory", cpkP+cpkM, cpElementCast, "CastorHistory", &history,
		"PROMISC", cpkP+cpkM, cpBool, &promisc,
        cpEnd);
}

void CastorSetAckNexthop::push(int, Packet* p) {

	const PacketId& pid = (PacketId&) *CastorPacket::getCastorAnno(p);
	bool use_broadcast = history->getPkts(pid) > 1;

	if (use_broadcast)
		p->set_dst_ip_anno(IPAddress::make_broadcast());
	else
		p->set_dst_ip_anno(history->getPktSenders(pid)[0]);

	if(promisc) {
		int randIndex = click_random() % history->getPkts(pid);
		CastorPacket::set_mac_ip_anno(p, history->getPktSenders(pid)[randIndex]);
	}

	output(use_broadcast).push(p);

}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorSetAckNexthop)
