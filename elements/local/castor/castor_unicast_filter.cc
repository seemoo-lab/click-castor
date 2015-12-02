#include <click/config.h>
#include <click/args.hh>
#include <click/hashtable.hh>
#include "castor_unicast_filter.hh"
#include "castor.hh"
#include "castor_xcast.hh"
#include "castor_anno.hh"

CLICK_DECLS

int CastorUnicastFilter::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_or_set_p("ACTIVE", active, false)
			.complete();
}

Packet* CastorUnicastFilter::simple_action(Packet *p) {
	if (!active)
		return p;

	if (CastorPacket::isXcast(p)) {

		bool isBroadcast = false;

		const CastorXcastPkt pkt(p);
		uint8_t index;
		for (index = 0; index < pkt.nnexthop(); index++) {
			if (pkt.nexthop(index) == NeighborId::make_broadcast()) {
				isBroadcast = true;
				break;
			}
		}

		if (!isBroadcast) {
			checked_output_push(1, pkt.getPacket());
			return 0;
		} else if (pkt.nexthop_assign(index) == pkt.ndst()) {
			return pkt.getPacket();
		} else {
			CastorXcastPkt pkt(p);
			// These destinations are broadcast
			Vector<uint8_t> destinations;
			pkt.nexthop_assigned_dsts(index, destinations);

			HashTable<uint8_t, uint8_t> toRemain;
			for(int i = 0; i < destinations.size(); i++)
				toRemain.set(destinations[i], destinations[i]);

			// Push PKT with unicast destinations
			CastorXcastPkt drop(pkt.getPacket()->clone()->uniqueify());
			pkt.remove(toRemain);
			pkt.set_single_nexthop(NeighborId());
			checked_output_push(1, drop.getPacket());

			// Return PKT with broadcast destinations
			pkt.keep(toRemain);
			pkt.set_single_nexthop(NeighborId::make_broadcast());
			return pkt.getPacket();
		}

	} else {
		if (CastorAnno::dst_id_anno(p) == NeighborId::make_broadcast()) {
			return p;
		} else {
			checked_output_push(1, p);
			return 0;
		}
	}
}

int CastorUnicastFilter::write_handler(const String &str, Element *e, void *, ErrorHandler *errh) {
	CastorUnicastFilter* filter = (CastorUnicastFilter*) e;

	bool active;
	if(Args(filter, errh).push_back_words(str)
			.read_p("ACTIVE", active)
			.complete() < 0)
		return -1;

	filter->active = active;
	return 0;
}

void CastorUnicastFilter::add_handlers() {
	add_write_handler("active", write_handler, 0);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorUnicastFilter)
