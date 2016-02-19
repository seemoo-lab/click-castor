#include <click/config.h>
#include <click/args.hh>
#include "castor_authenticate_pkt.hh"
#include "castor_xcast.hh"
#include "castor_anno.hh"

CLICK_DECLS

int CastorAuthenticatePkt::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_mp("Crypto", ElementCastArg("Crypto"), crypto)
			.complete();
}

void CastorAuthenticatePkt::push(int, Packet *p) {
	Hash pid;
	crypto->hash(pid, CastorAnno::hash_anno(p));
	bool isPidValid;

	if(CastorPacket::isXcast(p)) {
		CastorXcastPkt pkt = CastorXcastPkt(p);
		isPidValid = pid == pkt.pid(0); // Pkt should only include a single pid
	} else {
		CastorPkt& pkt = (CastorPkt&) *p->data();
		isPidValid = pid == pkt.pid;
	}

	if (isPidValid)
		output(0).push(p);
	else
		output(1).push(p); // Invalid -> discard
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorAuthenticatePkt)
