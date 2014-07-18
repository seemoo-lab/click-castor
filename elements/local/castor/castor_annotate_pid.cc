#include <click/config.h>
#include <click/confparse.hh>
#include "castor_annotate_pid.hh"

CLICK_DECLS

CastorAnnotatePid::CastorAnnotatePid() {
}

CastorAnnotatePid::~CastorAnnotatePid() {
}

int CastorAnnotatePid::configure(Vector<String>& conf, ErrorHandler* errh) {
    return cp_va_kparse(conf, this, errh,
    	"Crypto", cpkP+cpkM, cpElementCast, "Crypto", &crypto,
        cpEnd);
}

void CastorAnnotatePid::push(int, Packet* p) {

	// Compute the corresponding packet id
	Castor_ACK& ack = (Castor_ACK&) *p->data();
	PacketId pid;
	crypto->hash(pid, ack.auth, sizeof(ACKAuth));

	// Store it as packet annotation
	PacketId& pidAnno = CastorPacket::getPidAnnotationFromAck(p);
	memcpy(pidAnno, pid, sizeof(PacketId));

	output(0).push(p);

}


CLICK_ENDDECLS
EXPORT_ELEMENT(CastorAnnotatePid)
