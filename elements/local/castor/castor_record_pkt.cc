#include <click/config.h>
#include <click/straccum.hh>
#include "castor_record_pkt.hh"
#include "castor_xcast.hh"
#include "castor_anno.hh"

CLICK_DECLS

Packet* CastorRecordPkt::simple_action(Packet *p) {
	if(CastorPacket::getType(p) == CastorType::PKT) {
		if(CastorPacket::isXcast(p)) {
			CastorXcastPkt pkt = CastorXcastPkt(p);
			// Add all pids
			for(unsigned int i = 0; i < pkt.ndst(); i++) {
				records.push_back(new PidTime(pkt.pid(i)));
				npids++;
			}
			uint8_t nbroadcasts_old = nbroadcasts;
			// Add next hop decision for each destination
			for(unsigned int i = 0; i < pkt.nnexthop(); i++) {
				if(pkt.nexthop(i) == NeighborId::make_broadcast()) {
					nbroadcasts += pkt.nexthop_assign(i);
				} else {
					nunicasts += pkt.nexthop_assign(i);
				}
			}
			// We partially add packet size to size_broadcast and size_unicast
			uint8_t nbroadcasts_delta = nbroadcasts - nbroadcasts_old;
			size_t size_broadcast_delta = p->length() * nbroadcasts_delta / pkt.ndst();
			size_broadcast += size_broadcast_delta;
			size_unicast += p->length() - size_broadcast_delta;

#ifdef DEBUG_HOPCOUNT
			hopcounts.push_back(new UintListNode(pkt.hopcount()));
#endif
		} else {
			// Regular Castor PKT
			CastorPkt& pkt = (CastorPkt&) *p->data();
			records.push_back(new PidTime(pkt.pid));
			npids++;
			if(CastorAnno::dst_id_anno(p) == NeighborId::make_broadcast()) {
				size_broadcast += p->length();
				nbroadcasts++;
			} else {
				size_unicast += p->length();
				nunicasts++;
			}
#ifdef DEBUG_HOPCOUNT
			hopcounts.push_back(new UintListNode(pkt.hopcount));
#endif
		}
	} else if (CastorPacket::getType(p) == CastorType::ACK) {
		if(CastorAnno::dst_id_anno(p) == NeighborId::make_broadcast())
			size_broadcast += p->length();
		else
			size_unicast += p->length();
	} else {
		// Any other packet type
	}

	npackets++;
	size += p->length();
	size_noreset += p->length();

    return p;
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
	case Statistics::size_broadcast:
		return String(readAndReset(recorder->size_broadcast));
	case Statistics::size_unicast:
		return String(readAndReset(recorder->size_unicast));
	case Statistics::size_noreset:
		return String(recorder->size_noreset);
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
			sa << entry->pid.str() << " " << entry->time;
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
	add_read_handler("size_broadcast", read_handler, Statistics::size_broadcast);
	add_read_handler("size_unicast", read_handler, Statistics::size_unicast);
	add_read_handler("size_noreset", read_handler, Statistics::size_noreset);
	add_read_handler("nbroadcasts", read_handler, Statistics::nbroadcasts);
	add_read_handler("nunicasts", read_handler, Statistics::nunicasts);
	add_read_handler("seq_entry", read_handler, Statistics::seq_entry);
	add_read_handler("seq_hopcount", read_handler, Statistics::seq_hopcount);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorRecordPkt)
