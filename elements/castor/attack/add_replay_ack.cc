#include <click/config.h>
#include <click/args.hh>

#include "add_replay_ack.hh"
#include "../castor_anno.hh"

CLICK_DECLS

int AddReplayAck::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_mp("STORE", ElementCastArg("ReplayStore"), store)
			.complete();
}

Packet* AddReplayAck::simple_action(Packet *p) {
	const Hash& pid = CastorAnno::hash_anno(p);
	Packet* clone = p->clone();
	CastorAnno::dst_id_anno(clone) = NeighborId::make_broadcast();
	CastorAnno::hop_id_anno(clone) = NeighborId::make_broadcast();
	store->add_ack(pid, clone);
	return p;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(AddReplayAck)
