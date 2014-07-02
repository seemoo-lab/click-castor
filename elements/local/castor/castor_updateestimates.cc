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
		"CastorRoutingTable", cpkP+cpkM, cpElementCast, "CastorRoutingTable", &_table,
		"CastorHistory", cpkP+cpkM, cpElementCast, "CastorHistory", &_history,
        cpEnd);
}

void CastorUpdateEstimates::push(int, Packet *p){

	// The Address Annotation should contain the Source IP
	IPAddress src = p->dst_ip_anno();

	// Determine Destination of the origin Packet
	IPAddress dest = _history->PKTroutedto(p);

	// Determine the Flow ID
	FlowId fid;
	_history->GetFlowId(p, &fid);

	if (dest == IPAddress::make_broadcast()) {
		// PKT was broadcast
		if (_history->IsFirstACK(p)){
			_table->updateEstimates(fid, src,
					CastorRoutingTable::increase,
					CastorRoutingTable::first);
			_table->updateEstimates(fid, src,
					CastorRoutingTable::increase,
					CastorRoutingTable::all);
		} else {
			_table->updateEstimates(fid, src,
					CastorRoutingTable::increase,
					CastorRoutingTable::all);
		}
	} else if (src == dest) {
		// PKT was unicast, increase all estimates
		_table->updateEstimates(fid, src,
				CastorRoutingTable::increase,
				CastorRoutingTable::first);
		_table->updateEstimates(fid, src,
				CastorRoutingTable::increase,
				CastorRoutingTable::all);
	} else {
		// We have never forwarded this PKT
		output(1).push(p); // -> discard
		return;
	}

    output(0).push(p);

}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorUpdateEstimates)
