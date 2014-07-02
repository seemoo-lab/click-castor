#include <click/config.h>
#include <click/confparse.hh>
#include <click/straccum.hh>
#include "castor_lookuproute.hh"
#include <click/etheraddress.hh>

CLICK_DECLS

CastorLookupRoute::CastorLookupRoute() {
}

CastorLookupRoute::~CastorLookupRoute() {
}

int CastorLookupRoute::configure(Vector<String> &conf, ErrorHandler *errh) {
    return cp_va_kparse(conf, this, errh,
		"CastorRoutingTable", cpkP+cpkM, cpElementCast, "CastorRoutingTable", &_table,
        cpEnd);
}

void CastorLookupRoute::push(int, Packet *p){
	IPAddress nextHop;
	Castor_PKT header;

    // Copy the header from packet
    memcpy(&header, p->data(), sizeof(Castor_PKT));

	//Lookup
	nextHop = _table->lookup(header.fid);

	//Set annotation for destination and push Packet to Output
	p->set_dst_ip_anno(nextHop);

    output(0).push(p);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorLookupRoute)
