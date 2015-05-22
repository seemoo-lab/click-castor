#include <click/config.h>
#include <click/confparse.hh>
#include "castor_annotate_pid.hh"
#include "castor_xcast.hh"

CLICK_DECLS

CastorAnnotatePid::CastorAnnotatePid() {
	crypto = 0;
}

int CastorAnnotatePid::configure(Vector<String>& conf, ErrorHandler* errh) {
    return cp_va_kparse(conf, this, errh,
    	"Crypto", cpkP+cpkM, cpElementCast, "Crypto", &crypto,
        cpEnd);
}

void CastorAnnotatePid::push(int, Packet* p) {

	// Compute the corresponding packet id
	CastorAck& ack = (CastorAck&) *p->data();
	SValue pid = crypto->hash(SValue(ack.auth.data(), ack.hsize));

	// Store it as packet annotation
	PacketId& pidAnno = (PacketId&) *CastorPacket::getCastorAnno(p);
	pidAnno = PacketId(pid.begin());

	output(0).push(p);

}


CLICK_ENDDECLS
EXPORT_ELEMENT(CastorAnnotatePid)
