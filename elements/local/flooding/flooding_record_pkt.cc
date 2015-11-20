#include <click/config.h>
#include <click/args.hh>
#include "flooding_record_pkt.hh"
#include "flooding.hh"

CLICK_DECLS

int FloodingRecordPkt::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, errh)
			.read_mp("MAP", ElementCastArg("CastorXcastDestinationMap"), map)
			.complete();
}

void FloodingRecordPkt::push(int, Packet *p) {
	uint8_t recId[sizeof(PacketId)];
	memset(&recId, 0, sizeof(recId));
	Flooding::Id id = Flooding::getId(p);
	memcpy(&recId, &id, sizeof(Flooding::Id));
	PacketId castorPid = PacketId(recId); // Convert to Castor's pid type
	records.push_back(new PidTime(castorPid));

	hopcounts.push_back(new UintListNode(Flooding::getHopcount(p)));

	// TODO this is a hack so that we can count npids for multicast packets that are flooded
	GroupId group = IPAddress(p->ip_header()->ip_dst);
	unsigned int ndestinations = map->get(group).size();
	nbroadcasts += ndestinations;
	npids += ndestinations;

	npackets++;
	size += p->length();
	size_broadcast += p->length();

    output(0).push(p);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(FloodingRecordPkt)
