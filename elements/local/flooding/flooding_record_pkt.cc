#include <click/config.h>
#include <click/confparse.hh>
#include <click/straccum.hh>
#include "flooding_record_pkt.hh"
#include "flooding.hh"

CLICK_DECLS

int FloodingRecordPkt::configure(Vector<String> &conf, ErrorHandler *errh) {
	return cp_va_kparse(conf, this, errh,
		"MAP", cpkP+cpkM, cpElementCast, "CastorXcastDestinationMap", &map,
		cpEnd);
}

void FloodingRecordPkt::push(int, Packet *p) {
	IPAddress dst(p->ip_header()->ip_dst);
	unsigned int size = map->getDestinations(dst).size();

	if(p->dst_ip_anno() == IPAddress::make_broadcast())
		broadcastDecisions += size;
	records.push_back(Entry(Flooding::getId(p)));

	hopcounts.push_back(new ListNode(Flooding::getHopcount(p)));

	numPids += size;
	numPkts++;
	pktAccumSize += p->length();

    output(0).push(p);
}

String FloodingRecordPkt::read_handler(Element *e, void *thunk) {
	FloodingRecordPkt* recorder = static_cast<FloodingRecordPkt*>(e);

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
			sa << recorder->records[recorder->seq_index].id;
			sa << " " << recorder->records[recorder->seq_index].time;
			recorder->seq_index++;
			return sa.take_string();
		}
	case Statistics::seq_hopcount:
		if (recorder->hopcounts.empty())
			return String(-1); // no more entries
		else {
			ListNode* front = recorder->hopcounts.front();
			uint32_t val = front->value.value();
			recorder->hopcounts.pop_front();
			delete front;
			return String(val);
		}
	default:
		click_chatter("enum error");
		return String();
	}
}

void FloodingRecordPkt::add_handlers() {
	add_read_handler("num", read_handler, Statistics::num);
	add_read_handler("numUnique", read_handler, Statistics::numUnique);
	add_read_handler("size", read_handler, Statistics::size);
	add_read_handler("broadcasts", read_handler, Statistics::broadcasts);
	add_read_handler("unicasts", read_handler, Statistics::unicasts);
	add_read_handler("seq_entry", read_handler, Statistics::seq_entry);
	add_read_handler("seq_hopcount", read_handler, Statistics::seq_hopcount);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(FloodingRecordPkt)
