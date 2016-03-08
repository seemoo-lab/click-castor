#include <click/config.h>
#include <click/args.hh>
#include "castor_is_arq.hh"
#include "../castor.hh"

CLICK_DECLS

Packet* CastorIsARQ::simple_action(Packet *p) {
	if (reinterpret_cast<const CastorPkt*>(p->data())->arq) {
		output(1).push(p);
		return 0;
	} else {
		return p;
	}
}

CLICK_ENDDECLS
ELEMENT_REQUIRES(MerkleTree)
EXPORT_ELEMENT(CastorIsARQ)
