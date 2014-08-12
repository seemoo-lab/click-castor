#include <click/config.h>
#include <click/confparse.hh>
#include <click/straccum.hh>
#include "castor_record_pkt.hh"
#include "castor_xcast.hh"

CLICK_DECLS

CastorRecordPkt::CastorRecordPkt() {
	numPkts = 0;
	numPids = 0;
	pktAccumSize = 0;
	broadcastDecisions = 0;
	seq_index = 0;
}

CastorRecordPkt::~CastorRecordPkt() {
}

void CastorRecordPkt::push(int, Packet *p) {

	if(CastorPacket::getType(p) == CastorType::PKT) {
		if(CastorPacket::isXcast(p)) {
			CastorXcastPkt pkt = CastorXcastPkt(p);

			for(unsigned int i = 0; i < pkt.getNDestinations(); i++) {
				Entry newEntry(pkt.getPid(i));
				records.push_back(newEntry);
				numPids++;
			}

			for(unsigned int i = 0; i < pkt.getNNextHops(); i++) {
				if(pkt.getNextHop(i) == IPAddress::make_broadcast())
					broadcastDecisions += pkt.getNextHopNAssign(i);
			}
		} else {
			// Regular Castor PKT
			Castor_PKT& pkt = (Castor_PKT&) *p->data();
			Entry newEntry(pkt.pid);
			records.push_back(newEntry);
			numPids++;
			if(p->dst_ip_anno() == IPAddress::make_broadcast())
				broadcastDecisions++;
		}
	} else { // CastorType::ACK
		broadcastDecisions++;
	}

	numPkts++;
	pktAccumSize += p->length();

    output(0).push(p);
}

String CastorRecordPkt::read_handler(Element *e, void *thunk) {
	CastorRecordPkt* recorder = static_cast<CastorRecordPkt*>(e);

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
	case Statistics::seq_entry:
		if(recorder->seq_index >= recorder->records.size()) {
			return String(); // no more entries
		} else {
			StringAccum sa;
			sa << CastorPacket::hexToString(recorder->records[recorder->seq_index].pid, sizeof(Hash));
			sa << " " << recorder->records[recorder->seq_index].time;
			recorder->seq_index++;
			return sa.take_string();
		}
		// TODO implement more statistics
	default:
		click_chatter("enum error");
		return String();
	}
}

void CastorRecordPkt::add_handlers() {
	add_read_handler("num", read_handler, Statistics::num);
	add_read_handler("numUnique", read_handler, Statistics::numUnique);
	add_read_handler("size", read_handler, Statistics::size);
	add_read_handler("broadcasts", read_handler, Statistics::broadcasts);
	add_read_handler("unicasts", read_handler, Statistics::unicasts);
	add_read_handler("seq_entry", read_handler, Statistics::seq_entry);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorRecordPkt)
