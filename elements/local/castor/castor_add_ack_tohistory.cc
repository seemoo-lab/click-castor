#include <click/config.h>
#include <click/confparse.hh>
#include "castor_add_ack_tohistory.hh"

CLICK_DECLS

CastorAddACKToHistory::CastorAddACKToHistory() {
}

CastorAddACKToHistory::~CastorAddACKToHistory() {
}

int CastorAddACKToHistory::configure(Vector<String> &conf, ErrorHandler *errh) {
	return cp_va_kparse(conf, this, errh,
			"Crypto", cpkP+cpkM, cpElementCast, "Crypto", &crypto,
			"CastorHistory", cpkP+cpkM, cpElementCast, "CastorHistory", &history,
			cpEnd);
}

void CastorAddACKToHistory::push(int, Packet *p){
	Castor_ACK& ack = (Castor_ACK&) *p->data();

	PacketId pid;
	crypto->hash(pid, ack.auth, sizeof(ACKAuth));
	IPAddress prevHop = p->dst_ip_anno();

	history->addAckFor(pid, prevHop);

	output(0).push(p);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorAddACKToHistory)
