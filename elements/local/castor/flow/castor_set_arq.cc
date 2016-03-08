#include <click/config.h>
#include <click/args.hh>
#include "castor_set_arq.hh"
#include "../castor.hh"

CLICK_DECLS

int CastorSetARQ::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_mp("ARQ", _arq)
			.complete();
}

Packet* CastorSetARQ::simple_action(Packet *p) {
	if (reinterpret_cast<const CastorPkt*>(p->data())->arq == _arq) {
		return p;
	} else {
		WritablePacket* q = p->uniqueify();
		reinterpret_cast<CastorPkt*>(q->data())->arq = _arq;
		return q;
	}
}

CLICK_ENDDECLS
ELEMENT_REQUIRES(MerkleTree)
EXPORT_ELEMENT(CastorSetARQ)
