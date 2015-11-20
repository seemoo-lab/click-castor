#include <click/config.h>
#include <click/args.hh>
#include <click/hashtable.hh>
#include "castor_xcast_check_duplicate.hh"
#include "castor_xcast.hh"
#include "castor_anno.hh"

CLICK_DECLS

int CastorXcastCheckDuplicate::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, errh)
			.read_mp("CastorHistory", ElementCastArg("CastorHistory"), history)
			.read_mp("ID", myId)
			.complete();
}

void CastorXcastCheckDuplicate::push(int, Packet *p) {
	CastorXcastPkt pkt = CastorXcastPkt(p);

	/**
	 * XXX: According to Castor technical paper: If a packet with same pid, but different eauth or payload is received, it should not be considered a duplicate.
	 * In that case, however, the timer should not be restarted.
	 */

	HashTable<uint8_t, uint8_t> unseen;
	HashTable<uint8_t, uint8_t> alreadySeen;
	HashTable<uint8_t, uint8_t> retransmitAckTo;
	HashTable<uint8_t, uint8_t> addToHistory;

	for(unsigned int i = 0; i < pkt.getNDestinations(); i++)
		if(history->hasPkt(pkt.getPid(i))) {
			if (history->hasPktFrom(pkt.getPid(i), CastorAnno::src_id_anno(p))) {
				alreadySeen.set(i, i); // Already received Pid from this neighbor -> discard
			} else if (history->hasAck(pkt.getPid(i))) {
				retransmitAckTo.set(i, i); // Have not received Pid from this neighbor before AND already know corresponding ACK
			} else {
				addToHistory.set(i, i); // Have not received Pid from this neighbor before AND do NOT know corresponding ACK
			}
		} else {
			unseen.set(i, i); // New Pid
		}

	bool retransmitAck = retransmitAckTo.size() > 0;
	if(retransmitAck) {
		CastorXcastPkt copy = CastorXcastPkt(pkt.getPacket()->clone()->uniqueify());
		copy.keepDestinations(retransmitAckTo);
		copy.setSingleNextHop(myId);
		output(1).push(copy.getPacket());
	}

	bool toHistory = addToHistory.size() > 0;
	if (toHistory) {
		CastorXcastPkt copy = CastorXcastPkt(pkt.getPacket()->clone()->uniqueify());
		copy.keepDestinations(addToHistory);
		copy.setSingleNextHop(myId);
		output(2).push(copy.getPacket());
	}

	bool discard = alreadySeen.size() > 0;
	if(discard) {
		CastorXcastPkt copy = CastorXcastPkt(pkt.getPacket()->clone()->uniqueify());
		copy.keepDestinations(alreadySeen);
		copy.setSingleNextHop(myId);
		output(3).push(copy.getPacket());
	}

	bool isNew = unseen.size() > 0;
	if (isNew) {
		CastorXcastPkt copy = CastorXcastPkt(pkt.getPacket()->clone()->uniqueify());
		copy.keepDestinations(unseen);
		copy.setSingleNextHop(myId);
		output(0).push(copy.getPacket());
	}

	pkt.getPacket()->kill();
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorXcastCheckDuplicate)
