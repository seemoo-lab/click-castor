#include <click/config.h>
#include <click/confparse.hh>
#include "castor_authenticate_pkt.hh"
#include "castor_xcast.hh"

CLICK_DECLS

int CastorAuthenticatePkt::configure(Vector<String> &conf, ErrorHandler *errh) {
	return cp_va_kparse(conf, this, errh,
		"CRYPT", cpkP+cpkM, cpElementCast, "Crypto", &crypto,
		cpEnd);
}

void CastorAuthenticatePkt::push(int, Packet *p) {

	bool isPidValid;

	if(CastorPacket::isXcast(p)) {
		CastorXcastPkt pkt = CastorXcastPkt(p);

		const PacketId& computedPid = crypto->hash(CastorPacket::getCastorAnno(p));

		isPidValid = computedPid == pkt.getPid(0); // Pkt should only include a single pid
	} else {
		CastorPkt& pkt = (CastorPkt&) *p->data();

		const PacketId& computedPid = crypto->hash(CastorPacket::getCastorAnno(p));

		isPidValid = (computedPid == pkt.pid);
	}

	if (isPidValid)
		output(0).push(p);
	else
		output(1).push(p); // Invalid -> discard

}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorAuthenticatePkt)
