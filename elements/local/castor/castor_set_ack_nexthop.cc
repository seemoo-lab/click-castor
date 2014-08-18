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
        cpEnd);
}

void CastorSetAckNexthop::push(int, Packet* p) {

	const PacketId& pid = (PacketId&) *CastorPacket::getCastorAnno(p);

	if (_history->getPkts(pid) == 1) {
		p->set_dst_ip_anno(_history->getPktSenders(pid).at(0));
		if(_history->getPktSenders(pid).at(0).addr() == 0) {
			const Vector<IPAddress>& ph = _history->getPktSenders(pid);
			StringAccum sa;
			for(int i = 0; i < ph.size(); i++) {
				sa << ph[i].unparse().c_str() << ", ";
			}
			click_chatter("%s", sa.c_str());

		}
	}
	else
		p->set_dst_ip_anno(IPAddress::make_broadcast());

	output(0).push(p);

}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorSetAckNexthop)
