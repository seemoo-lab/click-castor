#include <click/config.h>
#include <click/confparse.hh>
#include <click/packet.hh>
#include <click/ipaddress.hh>
#include <click/straccum.hh>
#include "castor_set_ack_nexthop.hh"
#include "castor_xcast.hh"

CLICK_DECLS

CastorSetAckNexthop::CastorSetAckNexthop() {
}

CastorSetAckNexthop::~CastorSetAckNexthop() {
}

int CastorSetAckNexthop::configure(Vector<String> &conf, ErrorHandler *errh) {
    return cp_va_kparse(conf, this, errh,
		"CastorHistory", cpkP+cpkM, cpElementCast, "CastorHistory", &_history,
		"PROMISC", cpkP+cpkM, cpBool, &_promisc,
        cpEnd);
}

void CastorSetAckNexthop::push(int, Packet* p) {

	const PacketId& pid = (PacketId&) *CastorPacket::getCastorAnno(p);

	if (_history->getPkts(pid) == 1)
		p->set_dst_ip_anno(_history->getPktSenders(pid)[0]);
	else
		p->set_dst_ip_anno(IPAddress::make_broadcast());

	if(_promisc) {
		int randIndex = click_random() % _history->getPkts(pid);
		CastorPacket::set_mac_ip_anno(p, _history->getPktSenders(pid)[randIndex]);
	}

	output(0).push(p);

}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorSetAckNexthop)
