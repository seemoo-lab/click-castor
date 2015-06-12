#include <click/config.h>
#include <click/confparse.hh>
#include "castor_update_continuous_flow.hh"

CLICK_DECLS

int CastorUpdateContinuousFlow::configure(Vector<String> &conf, ErrorHandler *errh) {
    if (cp_va_kparse(conf, this, errh,
		"CastorNextFlowTable", cpkP+cpkM, cpElementCast, "CastorNextFlowTable", &fidTable,
		"CastorHistory", cpkP+cpkM, cpElementCast, "CastorHistory", &history,
		"CRYPT", cpkP+cpkM, cpElementCast, "Crypto", &crypto,
        cpEnd) < 0)
    	return -1;
    return 0;
}

void CastorUpdateContinuousFlow::push(int, Packet *p) {
	const PacketId& pid = CastorPacket::getCastorAnno(p);

	// XXX we assume here that the ACK was validated, such that we can be sure
	//     that the NextFlowAuth belonging to PID is valid
	fidTable->set(history->getNextFlowAuth(pid), history->getFlowId(pid), history->getDestination(pid));

	output(0).push(p);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorUpdateContinuousFlow)
