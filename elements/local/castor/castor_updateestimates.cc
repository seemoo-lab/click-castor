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
	Castor_ACK& ack = (Castor_ACK&) *p->data();

	PacketId pid;
	crypto->hash(pid, ack.auth, sizeof(ACKAuth));
	const FlowId& fid = _history->getFlowId(pid);

	const IPAddress& routedTo = _history->routedTo(pid);
	const IPAddress from = p->dst_ip_anno();

	if (routedTo == IPAddress::make_broadcast()) {
		// PKT was broadcast
		bool isFirstAck = _history->getACKs(pid) == 0;
		if (isFirstAck)
			_table->updateEstimates(fid, from, CastorRoutingTable::increase, CastorRoutingTable::first);
		_table->updateEstimates(fid, from, CastorRoutingTable::increase, CastorRoutingTable::all);
	} else if (routedTo == from) {
		// PKT was unicast
		_table->updateEstimates(fid, from, CastorRoutingTable::increase, CastorRoutingTable::first);
		_table->updateEstimates(fid, from, CastorRoutingTable::increase, CastorRoutingTable::all);
	} else {
		output(1).push(p); // received from wrong neighbor -> discard
		return;
	}

    output(0).push(p);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorUpdateEstimates)
