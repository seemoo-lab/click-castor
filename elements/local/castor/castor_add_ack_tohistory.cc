#include <click/config.h>
#include <click/confparse.hh>
#include <click/args.hh>
#include <click/error.hh>
#include "castor_add_ack_tohistory.hh"
#include "castor_xcast.hh"

CLICK_DECLS

CastorAddAckToHistory::CastorAddAckToHistory() {
}

CastorAddAckToHistory::~CastorAddAckToHistory() {
}

int CastorAddAckToHistory::configure(Vector<String> &conf, ErrorHandler *errh) {
	String dst_str;

	if(cp_va_kparse(conf, this, errh,
			"Crypto", cpkP+cpkM, cpElementCast, "Crypto", &crypto,
			"CastorHistory", cpkP+cpkM, cpElementCast, "CastorHistory", &history,
			"PrevHop", cpkP+cpkM, cpString, &dst_str,
			cpEnd) < 0)
		return -1;

	useDstAnno = dst_str == "DST_ANNO";
	if (useDstAnno)
		myAddr = IPAddress(0);
	else if (!IPAddressArg::parse(dst_str, myAddr, this))
		return errh->error("PrevHop argument should be IP address of this node or 'DST_ANNO'");

	return 0;
}

void CastorAddAckToHistory::push(int, Packet *p) {

	PacketId pid;

	if(CastorPacket::isXcast(p)) {
		CastorXcastAck& ack = (CastorXcastAck&) *p->data();
		crypto->hash(pid, ack.auth, ack.esize);
	} else {
		Castor_ACK& ack = (Castor_ACK&) *p->data();
		crypto->hash(pid, ack.auth, ack.hsize);
	}

	history->addAckFor(pid, useDstAnno ? p->dst_ip_anno() : myAddr);

	output(0).push(p);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorAddAckToHistory)
