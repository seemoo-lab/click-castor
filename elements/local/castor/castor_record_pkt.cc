#include <click/config.h>
#include <click/confparse.hh>
#include "castor_record_pkt.hh"
#include "castor_xcast.hh"

CLICK_DECLS

int CastorRecordPkt::configure(Vector<String> &conf, ErrorHandler *errh) {
     int result = cp_va_kparse(conf, this, errh,
        "Interval", cpkP, cpTimestamp, &interval,
        cpEnd);
     currentIntervalEnd = interval;
     return result;
}

void CastorRecordPkt::push(int, Packet *p) {

	// Fast forward to current interval entry
	for (; currentIntervalEnd < Timestamp::now_steady() || sizeInterval.back() == 0; currentIntervalEnd += interval) {
		ListNode* node = new ListNode();
		sizeInterval.push_back(node);
	}

	if(CastorPacket::getType(p) == CastorType::PKT) {
		if(CastorPacket::isXcast(p)) {
			CastorXcastPkt pkt = CastorXcastPkt(p);
			// Add all pids
			for(unsigned int i = 0; i < pkt.getNDestinations(); i++) {
				Entry newEntry(pkt.getPid(i));
				records.push_back(newEntry);
				numPids++;
			}
			// Add next hop decision for each destination
			for(unsigned int i = 0; i < pkt.getNNextHops(); i++) {
				if(pkt.getNextHop(i) == NodeId::make_broadcast())
					broadcastDecisions += pkt.getNextHopNAssign(i);
			}
			hopcounts.push_back(pkt.getHopcount());
		} else {
			// Regular Castor PKT
			Castor_PKT& pkt = (Castor_PKT&) *p->data();
			Entry newEntry(pkt.pid);
			records.push_back(newEntry);
			numPids++;
			if(p->dst_ip_anno() == NodeId::make_broadcast())
				broadcastDecisions++;
			hopcounts.push_back(pkt.hopcount);
		}
	} else { // CastorType::ACK
		broadcastDecisions++;
	}

	numPkts++;
	pktAccumSize += p->length();
	sizeInterval.back()->value += p->length();

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
	case Statistics::seq_hopcount:
		if(recorder->hopcount_index >= recorder->hopcounts.size()) {
			return String(-1); // no more entries
		} else {
			int hopcount = recorder->hopcounts[recorder->hopcount_index];
			recorder->hopcount_index++;
			return String(hopcount);
		}
	case Statistics::size_interval:
		if (recorder->sizeInterval.empty())
			return String(-1); // no more entries
		else {
			ListNode* front = recorder->sizeInterval.front();
			uint32_t val = front->value.value();
			recorder->sizeInterval.pop_front();
			delete front;
			return String(val);
		}
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
	add_read_handler("seq_hopcount", read_handler, Statistics::seq_hopcount);
	add_read_handler("size_interval", read_handler, Statistics::size_interval);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorRecordPkt)
