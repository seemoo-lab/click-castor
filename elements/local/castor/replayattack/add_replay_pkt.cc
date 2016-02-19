#include <click/config.h>
#include <click/args.hh>

#include "add_replay_pkt.hh"
#include "../castor.hh"
#include "../castor_anno.hh"

CLICK_DECLS

int AddReplayPkt::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_mp("STORE", ElementCastArg("ReplayStore"), store)
			.complete();
}

Packet* AddReplayPkt::simple_action(Packet *p) {
	CastorPkt& pkt = (CastorPkt&) *p->data();
	WritablePacket* clone = p->clone()->uniqueify();
	clone->take(pkt.payload_len());
	reinterpret_cast<CastorPkt&>(*clone->data()).len = htons(pkt.header_len());
	CastorAnno::dst_id_anno(clone) = NeighborId::make_broadcast();
	CastorAnno::hop_id_anno(clone) = NeighborId::make_broadcast();
	store->add_pkt(pkt.pid, clone);
	return p;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(AddReplayPkt)
