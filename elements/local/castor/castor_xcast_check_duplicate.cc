#include <click/config.h>
#include <click/confparse.hh>

#include "castor_xcast_check_duplicate.hh"
#include "castor_xcast.hh"

#include <click/hashtable.hh>

CLICK_DECLS

CastorXcastCheckDuplicate::CastorXcastCheckDuplicate() {
}

CastorXcastCheckDuplicate::~CastorXcastCheckDuplicate() {
}

int CastorXcastCheckDuplicate::configure(Vector<String> &conf, ErrorHandler *errh) {
     return cp_va_kparse(conf, this, errh,
        "CastorHistory", cpkP+cpkM, cpElementCast, "CastorHistory", &history,
        "ADDR", cpkP+cpkM, cpIPAddress, &myAddr,
        cpEnd);
}

void CastorXcastCheckDuplicate::push(int, Packet *p) {
	CastorXcastPkt pkt = CastorXcastPkt(p);

	HashTable<uint8_t, uint8_t> alreadySeen;
	HashTable<uint8_t, uint8_t> retransmitAckTo;

	for(unsigned int i = 0; i < pkt.getNDestinations(); i++)
		if(history->hasPkt(pkt.getPid(i))) {
			alreadySeen.set(i, i);
			if(!history->hasPktFrom(pkt.getPid(i), CastorPacket::src_ip_anno(p)) && history->hasAck(pkt.getPid(i)))
				retransmitAckTo.set(i, i);
		}

	bool retransmitAck = retransmitAckTo.size() > 0;
	if(retransmitAck) {
		CastorXcastPkt copy = CastorXcastPkt(pkt.getPacket()->clone()->uniqueify());
		copy.keepDestinations(retransmitAckTo);
		copy.setSingleNextHop(myAddr);
		output(1).push(copy.getPacket());
	}

	bool isDuplicate = pkt.getNDestinations() == alreadySeen.size();
	if(isDuplicate) {
		/**
		 * XXX: According to Castor technical paper: If a packet with same pid, but different eauth or payload is received, it should not be considered a duplicate.
		 * In that case, however, the timer should not be restarted.
		 */
		output(2).push(pkt.getPacket()); // -> discard
	} else {
		// Remove destinations
		pkt.removeDestinations(alreadySeen);
		pkt.setSingleNextHop(myAddr);
		output(0).push(pkt.getPacket());
	}
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorXcastCheckDuplicate)
