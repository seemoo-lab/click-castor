#include <click/config.h>
#include <click/confparse.hh>
#include <click/straccum.hh>
#include <click/etheraddress.hh>
#include "castor_updateestimates.hh"

CLICK_DECLS

CastorUpdateEstimates::CastorUpdateEstimates() {
}

CastorUpdateEstimates::~CastorUpdateEstimates() {
}

int CastorUpdateEstimates::configure(Vector<String> &conf, ErrorHandler *errh) {
    return cp_va_kparse(conf, this, errh,
        "Crypto", cpkP+cpkM, cpElementCast, "Crypto", &crypto,
		"CastorRoutingTable", cpkP+cpkM, cpElementCast, "CastorRoutingTable", &_table,
		"CastorHistory", cpkP+cpkM, cpElementCast, "CastorHistory", &_history,
        cpEnd);
}

void CastorUpdateEstimates::push(int, Packet *p){

	const PacketId& pid = CastorPacket::getPidAnnotationFromAck(p);

	const FlowId& fid = _history->getFlowId(pid);
	const IPAddress& routedTo = _history->routedTo(pid);
	const IPAddress from = p->dst_ip_anno();
	bool isFirstAck = _history->getACKs(pid) == 0;

	if (routedTo == IPAddress::make_broadcast()) {
		// PKT was broadcast
		if (isFirstAck)
			_table->updateEstimates(fid, from, CastorRoutingTable::increase, CastorRoutingTable::first);
		_table->updateEstimates(fid, from, CastorRoutingTable::increase, CastorRoutingTable::all);
	} else if (routedTo == from) {
		// PKT was unicast
		_table->updateEstimates(fid, from, CastorRoutingTable::increase, CastorRoutingTable::first);
		_table->updateEstimates(fid, from, CastorRoutingTable::increase, CastorRoutingTable::all);
	} else {
		output(2).push(p); // received from wrong neighbor -> discard
		return;
	}

	if(isFirstAck) {
	    output(0).push(p); // only forward 1st ACK
	} else {
		output(1).push(p); // don't forward ACK again
	}
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorUpdateEstimates)
