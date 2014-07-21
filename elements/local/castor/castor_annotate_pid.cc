#include <click/config.h>
#include <click/confparse.hh>
#include "castor_annotate_pid.hh"
#include "castor_xcast.hh"

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
	PacketId pid;

	if(CastorPacket::isXcast(p)) {
		CastorXcastAck& ack = (CastorXcastAck&) *p->data();
		crypto->hash(pid, ack.auth, ack.esize);
	} else {
		Castor_ACK& ack = (Castor_ACK&) *p->data();
		crypto->hash(pid, ack.auth, ack.hsize);
	}

	// Store it as packet annotation
	PacketId& pidAnno = (PacketId&) *CastorPacket::getCastorAnno(p);
	memcpy(pidAnno, pid, sizeof(PacketId));

	output(0).push(p);

}


CLICK_ENDDECLS
EXPORT_ELEMENT(CastorAnnotatePid)
