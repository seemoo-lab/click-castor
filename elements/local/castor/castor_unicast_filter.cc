#include <click/config.h>
#include <click/args.hh>
#include <click/hashtable.hh>
#include "castor_unicast_filter.hh"
#include "castor_xcast.hh"
#include "castor_anno.hh"

CLICK_DECLS

int CastorUnicastFilter::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_p("ACTIVE", active)
			.complete();
}

void CastorUnicastFilter::push(int, Packet *p) {
	if (!active) {
		output(0).push(p);
		return;
	}

	if (CastorPacket::isXcast(p)) {

		bool isBroadcast = false;

		CastorXcastPkt pkt(p);
		uint8_t index;
		for (index = 0; index < pkt.getNNextHops(); index++) {
			if (pkt.getNextHop(index) == NeighborId::make_broadcast()) {
				isBroadcast = true;
				break;
			}
		}

		if (!isBroadcast) {
			output(1).push(pkt.getPacket());
		} else if (pkt.getNextHopNAssign(index) == pkt.getNDestinations()) {
			output(0).push(pkt.getPacket());
		} else {
			// These destinations are broadcast
			Vector<unsigned int> destinations;
			pkt.getNextHopDestinations(index, destinations);

			HashTable<uint8_t, uint8_t> toRemain;
			for(int i = 0; i < destinations.size(); i++) {
				toRemain.set(destinations[i], destinations[i]);
			}

			// Push PKT with broadcast destinations
			CastorXcastPkt local(pkt.getPacket()->clone()->uniqueify());
			local.keepDestinations(toRemain);
			local.setSingleNextHop(NeighborId::make_broadcast());
			output(0).push(local.getPacket());

			// Push PKT with unicast destinations
			pkt.removeDestinations(toRemain);
			pkt.setSingleNextHop(NeighborId());
			output(1).push(pkt.getPacket());
		}

	} else {

		if (CastorAnno::dst_id_anno(p) == NeighborId::make_broadcast())
			output(0).push(p); // Was forwarded to me as broadcast PKT
		else
			output(1).push(p); // Was unicast to me as broadcast PKT

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
