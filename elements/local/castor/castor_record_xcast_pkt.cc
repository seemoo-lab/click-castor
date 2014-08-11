#include <click/config.h>
#include <click/confparse.hh>
#include <click/straccum.hh>
#include "castor_record_xcast_pkt.hh"
#include "castor_xcast.hh"

CLICK_DECLS

CastorRecordXcastPkt::CastorRecordXcastPkt() {
	numPkts = 0;
	numPids = 0;
	pktAccumSize = 0;
	broadcastDecisions = 0;
}

CastorRecordXcastPkt::~CastorRecordXcastPkt() {
}

void CastorRecordXcastPkt::push(int, Packet *p){
	CastorXcastPkt pkt = CastorXcastPkt(p);

	for(unsigned int i = 0; i < pkt.getNDestinations(); i++) {
		Entry newEntry(pkt.getPid(i), pkt.getTotalLength());
		records.push_back(newEntry);
		numPids++;
	}

	for(unsigned int i = 0; i < pkt.getNNextHops(); i++) {
		if(pkt.getNextHop(i) == IPAddress::make_broadcast())
			broadcastDecisions++;
	}

	numPkts++;
	pktAccumSize += (uint32_t) pkt.getTotalLength();

    output(0).push(pkt.getPacket());
}

String CastorRecordXcastPkt::read_handler(Element *e, void *thunk) {
	CastorRecordXcastPkt* recorder = static_cast<CastorRecordXcastPkt*>(e);

	switch(reinterpret_cast<uintptr_t>(thunk)) {
	case Statistics::num:
		return String(recorder->numPids);
	case Statistics::numUnique:
		return String(recorder->numPkts);
	case Statistics::size:
		return String(recorder->pktAccumSize);
	case Statistics::broadcasts:
		return String(recorder->broadcastDecisions);
	case Statistics::unicasts:
		return String(recorder->numPids - recorder->broadcastDecisions);
		// TODO implement more statistics
	default:
		click_chatter("enum error");
		return String();
	}
}

void CastorRecordXcastPkt::add_handlers() {
	add_read_handler("num", read_handler, Statistics::num);
	add_read_handler("numUnique", read_handler, Statistics::numUnique);
	add_read_handler("size", read_handler, Statistics::size);
	add_read_handler("broadcasts", read_handler, Statistics::broadcasts);
	add_read_handler("unicasts", read_handler, Statistics::unicasts);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorRecordXcastPkt)
