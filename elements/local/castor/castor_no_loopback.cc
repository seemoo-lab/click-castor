#include <click/config.h>
#include <click/confparse.hh>
#include <click/packet.hh>
#include <click/ipaddress.hh>
#include "castor_no_loopback.hh"

CLICK_DECLS

CastorNoLoopback::CastorNoLoopback() {
}

CastorNoLoopback::~CastorNoLoopback() {
}

int CastorNoLoopback::configure(Vector<String> &conf, ErrorHandler *errh) {
    return cp_va_kparse(conf, this, errh,
		"ADDR", cpkP+cpkM, cpIPAddress, &myAddr,
        cpEnd);
}

void CastorNoLoopback::push(int, Packet* p) {

	if (p->dst_ip_anno() == myAddr)
		output(1).push(p);
	else
		output(0).push(p);

}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorNoLoopback)
