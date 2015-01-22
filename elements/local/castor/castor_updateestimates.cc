#include <click/config.h>
#include <click/confparse.hh>
#include <click/straccum.hh>
#include <click/etheraddress.hh>
#include "castor_updateestimates.hh"

CLICK_DECLS

CastorUpdateEstimates::CastorUpdateEstimates() {
	crypto = 0;
	table = 0;
	history = 0;
}

int CastorUpdateEstimates::configure(Vector<String> &conf, ErrorHandler *errh) {
    return cp_va_kparse(conf, this, errh,
        "Crypto", cpkP+cpkM, cpElementCast, "Crypto", &crypto,
		"CastorRoutingTable", cpkP+cpkM, cpElementCast, "CastorRoutingTable", &table,
		"CastorHistory", cpkP+cpkM, cpElementCast, "CastorHistory", &history,
        cpEnd);
}

void CastorUpdateEstimates::push(int, Packet *p){

	const PacketId& pid = (PacketId&) *CastorPacket::getCastorAnno(p);

	// TODO do all that with a single call
	const FlowId& fid = history->getFlowId(pid);
	IPAddress subfid = history->getDestination(pid);
	IPAddress routedTo = history->routedTo(pid);
	IPAddress from = CastorPacket::src_ip_anno(p);
	bool isFirstAck = !history->hasAck(pid);

	if (routedTo == IPAddress::make_broadcast()) {
		// PKT was broadcast
		if (isFirstAck)
			table->updateEstimates(fid, subfid, from, CastorRoutingTable::increase, CastorRoutingTable::first);
		table->updateEstimates(fid, subfid, from, CastorRoutingTable::increase, CastorRoutingTable::all);
	} else if (routedTo == from) {
		// PKT was unicast
		table->updateEstimates(fid, subfid, from, CastorRoutingTable::increase, CastorRoutingTable::first);
		table->updateEstimates(fid, subfid, from, CastorRoutingTable::increase, CastorRoutingTable::all);
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
