#include <click/config.h>
#include <click/confparse.hh>
#include "castor_validate_flow_atdest.hh"

CLICK_DECLS

CastorValidateFlowAtDestination::CastorValidateFlowAtDestination() {
}

CastorValidateFlowAtDestination::~CastorValidateFlowAtDestination() {
}

int CastorValidateFlowAtDestination::configure(Vector<String> &conf, ErrorHandler *errh) {
	return cp_va_kparse(conf, this, errh,
		"CRYPT", cpkP+cpkM, cpElementCast, "Crypto", &crypto,
		cpEnd);
}

void CastorValidateFlowAtDestination::push(int, Packet *p) {

	Castor_PKT* pkt = (Castor_PKT*) p->data();

	PacketId computedPid;
	crypto->hash(computedPid, CastorPacket::getCastorAnno(p), sizeof(PacketId)); // eauth should be already decrypted!

	bool isPidValid = (memcmp(computedPid, pkt->pid, sizeof(PacketId)) == 0);

	if (isPidValid)
		output(0).push(p);
	else
		output(1).push(p); // Invalid -> discard

}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorValidateFlowAtDestination)
