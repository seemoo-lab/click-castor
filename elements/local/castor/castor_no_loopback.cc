#include <click/config.h>
#include <click/confparse.hh>
#include <click/packet.hh>
#include "castor_no_loopback.hh"

CLICK_DECLS

int CastorNoLoopback::configure(Vector<String> &conf, ErrorHandler *errh) {
    return cp_va_kparse(conf, this, errh,
		"CastorHistory", cpkP+cpkM, cpElementCast, "CastorHistory", &history,
		"NodeId", cpkP+cpkM, cpIPAddress, &myId,
        cpEnd);
}

void CastorNoLoopback::push(int, Packet* p) {

	const PacketId& pid = CastorPacket::getCastorAnno(p);

	if(history->hasPktFrom(pid, myId))
		output(1).push(p);  // ACK arrived at source of corresponding PKT
	else
		output(0).push(p);  // ACK arrived at intermediate network node

}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorNoLoopback)
