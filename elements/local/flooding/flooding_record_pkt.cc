#include <click/config.h>
#include <click/args.hh>
#include "flooding_record_pkt.hh"
#include "flooding.hh"

CLICK_DECLS

int FloodingRecordPkt::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_mp("MAP", ElementCastArg("CastorXcastDestinationMap"), map)
			.complete();
}

Packet* FloodingRecordPkt::simple_action(Packet *p) {
	PacketId castorPid;
	memcpy(&castorPid,
		   &Flooding::id(p),
		   sizeof(Flooding::Id) > sizeof(PacketId) ? sizeof(PacketId) : sizeof(Flooding::Id));
	records.push_back(new PidTime(castorPid));

	hopcounts.push_back(new UintListNode(Flooding::hopcount(p)));

	// this is a hack so that we can count npids for multicast packets that are flooded
	GroupId group = IPAddress(p->ip_header()->ip_dst);
	unsigned int ndestinations = map->get(group).size();
	nbroadcasts += ndestinations;
	npids += ndestinations;

	npackets++;
	size += p->length();
	size_broadcast += p->length();

	return p;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(FloodingRecordPkt)
