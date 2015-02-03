#include <click/config.h>
#include <click/confparse.hh>
#include <click/straccum.hh>
#include <click/etheraddress.hh>
#include "castor_updateestimates.hh"

CLICK_DECLS

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

	if (routedTo == from || isFirstAck)
		table->updateEstimates(fid, subfid, from, CastorRoutingTable::increase, CastorRoutingTable::first);
	table->updateEstimates(fid, subfid, from, CastorRoutingTable::increase, CastorRoutingTable::all);

    output(!isFirstAck).push(p); // only forward 1st ACK on default output port 0
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorUpdateEstimates)
