#include <click/config.h>
#include <click/args.hh>
#include "neighbor_auth_check_icv.hh"
#include "../castor_anno.hh"
#include "../castor.hh"

CLICK_DECLS

int NeighborAuthCheckICV::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_mp("Crypto", ElementCastArg("Crypto"), crypto)
			.complete();
}

Packet* NeighborAuthCheckICV::simple_action(Packet *p) {
	if (p->length() < icv_BYTES) {
		// Packet is too short
		checked_output_push(1, p);
		return 0;
	}

	// FIXME currently ignoring ICV for broadcast packets
	if (CastorAnno::dst_id_anno(p).is_broadcast())
		return p;

	const NeighborId& node = CastorAnno::src_id_anno(p);
	const SymmetricKey* key = crypto->getSharedKey(node);
	if (!key) {
		click_chatter("Could not find shared key for host %s. Discarding PKT...", node.unparse().c_str());
		checked_output_push(1, p);
		return 0;
	}

	ICV calc;
	crypto->auth(calc, p->data(), p->length() - icv_BYTES, key->data());

	if (calc != ICV(p->end_data() - icv_BYTES)) {
		// ICV does not match -> invalid
		checked_output_push(1, p);
		return 0;
	}

	return p;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(NeighborAuthCheckICV)
