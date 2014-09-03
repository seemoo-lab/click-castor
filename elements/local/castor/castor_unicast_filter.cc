#include <click/config.h>
#include <click/args.hh>
#include <click/confparse.hh>
#include <click/hashtable.hh>
#include "castor_unicast_filter.hh"
#include "castor_xcast.hh"

CLICK_DECLS

CastorUnicastFilter::CastorUnicastFilter() {
}

CastorUnicastFilter::~CastorUnicastFilter() {
}

int CastorUnicastFilter::configure(Vector<String> &conf, ErrorHandler *errh) {
	active = false;

	if(Args(conf, this, errh)
			.read_mp("ADDR", myAddr)
			.read_p("ACTIVE", active)
			.complete() < 0)
		return -1;
	return 0;
}

void CastorUnicastFilter::push(int, Packet *p) {

	if (!active) {
		output(0).push(p);
		return;
	}

	if (CastorPacket::isXcast(p)) {

		bool isBroadcast = false;
		uint8_t index;

		CastorXcastPkt pkt(p);
		for (uint8_t i = 0; i < pkt.getNNextHops(); i++) {
			if (pkt.getNextHop(i) == IPAddress::make_broadcast()) {
				isBroadcast = true;
				index = i;
				break;
			}
		}

		if (!isBroadcast) {
			output(1).push(pkt.getPacket());
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
			local.setSingleNextHop(myAddr);
			output(0).push(local.getPacket());

			// Push PKT with unicast destinations
			pkt.removeDestinations(toRemain);
			pkt.setSingleNextHop(IPAddress());
			output(1).push(pkt.getPacket());
		}

	} else {

		if (p->dst_ip_anno() == IPAddress::make_broadcast())
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
