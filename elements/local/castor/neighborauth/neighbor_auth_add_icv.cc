#include <click/config.h>
#include <click/args.hh>
#include "neighbor_auth_add_icv.hh"
#include "../castor_anno.hh"
#include "../castor.hh"

CLICK_DECLS

int NeighborAuthAddICV::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_mp("Crypto", ElementCastArg("Crypto"), crypto)
			.complete();
}

Packet* NeighborAuthAddICV::simple_action(Packet *p) {
	const SymmetricKey* key = crypto->getSharedKey(CastorAnno::dst_id_anno(p));
	if (!key) {
		click_chatter("Could not find shared key for host %s. Discarding PKT...", CastorAnno::dst_id_anno(p).unparse().c_str());
		checked_output_push(1, p);
		return 0;
	}
	WritablePacket* q = p->put(icv_BYTES);
	if (!q)
		return 0;

	crypto->auth(q->end_data() - icv_BYTES, p->data(), p->length() - icv_BYTES, key->data());

	return q;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(NeighborAuthAddICV)
