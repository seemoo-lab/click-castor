#include <click/config.h>
#include <click/confparse.hh>
#include "castor_authenticate_pkt.hh"
#include "castor_xcast.hh"

CLICK_DECLS

CastorAuthenticatePkt::CastorAuthenticatePkt() {
}

CastorAuthenticatePkt::~CastorAuthenticatePkt() {
}

int CastorAuthenticatePkt::configure(Vector<String> &conf, ErrorHandler *errh) {
	return cp_va_kparse(conf, this, errh,
		"CRYPT", cpkP+cpkM, cpElementCast, "Crypto", &crypto,
		cpEnd);
}

void CastorAuthenticatePkt::push(int, Packet *p) {

	bool isPidValid;

	if(CastorPacket::isXcast(p)) {
		CastorXcastPkt pkt = CastorXcastPkt(p);

		PacketId computedPid;
		crypto->hash(computedPid, CastorPacket::getCastorAnno(p), sizeof(EACKAuth));

		isPidValid = (memcmp(computedPid, pkt.getPid(0), sizeof(PacketId)) == 0); // Pkt should only include a single pid
	} else {
		Castor_PKT* pkt = (Castor_PKT*) p->data();

		PacketId computedPid;
		crypto->hash(computedPid, CastorPacket::getCastorAnno(p), sizeof(ACKAuth));

		isPidValid = (memcmp(computedPid, pkt->pid, sizeof(PacketId)) == 0);
	}

	if (isPidValid)
		output(0).push(p);
	else
		output(1).push(p); // Invalid -> discard

}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorAuthenticatePkt)
