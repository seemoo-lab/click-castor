#include <click/config.h>
#include <click/args.hh>
#include "castor_mirror.hh"
#include "castor_anno.hh"

CLICK_DECLS

int CastorMirror::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_mp("ID", id)
			.complete();
}

Packet* CastorMirror::simple_action(Packet* p) {
	WritablePacket* q = p->uniqueify();
	CastorPkt& pkt = (CastorPkt&) *q->data();
	pkt.arq = 1;
	CastorAnno::dst_id_anno(p) = CastorAnno::src_id_anno(p);
	CastorAnno::hop_id_anno(p) = CastorAnno::src_id_anno(p);
	CastorAnno::src_id_anno(p) = id;
	return p;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorMirror)
