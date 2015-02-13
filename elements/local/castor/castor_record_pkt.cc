#include <click/config.h>
#include <click/confparse.hh>
#include "castor_record_pkt.hh"
#include "castor_xcast.hh"

CLICK_DECLS

void CastorRecordPkt::push(int, Packet *p) {
	if(CastorPacket::getType(p) == CastorType::PKT) {
		if(CastorPacket::isXcast(p)) {
			CastorXcastPkt pkt = CastorXcastPkt(p);
			// Add all pids
			for(unsigned int i = 0; i < pkt.getNDestinations(); i++) {
				records.push_back(new PidTime(pkt.getPid(i)));
				npids++;
			}
			// Add next hop decision for each destination
			for(unsigned int i = 0; i < pkt.getNNextHops(); i++) {
				if(pkt.getNextHop(i) == NodeId::make_broadcast())
					nbroadcasts += pkt.getNextHopNAssign(i);
				else
					nunicasts += pkt.getNextHopNAssign(i);
			}
			hopcounts.push_back(new UintListNode(pkt.getHopcount()));
		} else {
			// Regular Castor PKT
			Castor_PKT& pkt = (Castor_PKT&) *p->data();
			records.push_back(new PidTime(pkt.pid));
			npids++;
			if(p->dst_ip_anno() == NodeId::make_broadcast())
				nbroadcasts++;
			else
				nunicasts++;
			hopcounts.push_back(new UintListNode(pkt.hopcount));
		}
	}
	npackets++;
	size += p->length();

    output(0).push(p);
}

String CastorRecordPkt::read_handler(Element *e, void *thunk) {
	CastorRecordPkt* recorder = static_cast<CastorRecordPkt*>(e);

	switch(reinterpret_cast<uintptr_t>(thunk)) {
	case Statistics::npids:
		return String(readAndReset(recorder->npids));
	case Statistics::npackets:
		return String(readAndReset(recorder->npackets));
	case Statistics::size:
		return String(readAndReset(recorder->size));
	case Statistics::nbroadcasts:
		return String(readAndReset(recorder->nbroadcasts));
	case Statistics::nunicasts:
		return String(readAndReset(recorder->nunicasts));
	case Statistics::seq_entry:
		if(recorder->records.empty()) {
			return String(); // no more entries
		} else {
			PidTime* entry = recorder->records.front();
			StringAccum sa;
			sa << CastorPacket::hexToString(entry->pid, sizeof(Hash)) << " " << entry->time;
			recorder->records.pop_front();
			delete entry;
			return sa.take_string();
		}
	case Statistics::seq_hopcount:
		if (recorder->hopcounts.empty()) {
			return String();
		} else {
			UintListNode* entry = recorder->hopcounts.front();
			unsigned int hc = entry->value;
			recorder->hopcounts.pop_front();
			delete entry;
			return String(hc);
		}
	default:
		click_chatter("enum error");
		return String();
	}
}

void CastorRecordPkt::add_handlers() {
	add_read_handler("npids", read_handler, Statistics::npids);
	add_read_handler("npackets", read_handler, Statistics::npackets);
	add_read_handler("size", read_handler, Statistics::size);
	add_read_handler("nbroadcasts", read_handler, Statistics::nbroadcasts);
	add_read_handler("nunicasts", read_handler, Statistics::nunicasts);
	add_read_handler("seq_entry", read_handler, Statistics::seq_entry);
	add_read_handler("seq_hopcount", read_handler, Statistics::seq_hopcount);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorRecordPkt)
