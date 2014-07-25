#include <click/config.h>
#include <click/confparse.hh>

#include "castor_xcast_set_destinations.hh"
#include "castor_xcast.hh"

CLICK_DECLS

CastorXcastSetDestinations::CastorXcastSetDestinations() {
}

CastorXcastSetDestinations::~CastorXcastSetDestinations() {
}

int CastorXcastSetDestinations::configure(Vector<String> &conf, ErrorHandler *errh) {
     return cp_va_kparse(conf, this, errh,
    	"CRYPT", cpkP+cpkM, cpElementCast, "Crypto", &_crypto,
        "CastorXcastDestinationMap", cpkP+cpkM, cpElementCast, "CastorXcastDestinationMap", &_map,
        cpEnd);
}

void CastorXcastSetDestinations::push(int, Packet *p) {

	WritablePacket* q = p->uniqueify();
	CastorXcastPkt header = CastorXcastPkt(q);

	IPAddress multicastDst = header.getMulticastGroup();
	const Vector<IPAddress>& destinations = _map->getDestinations(multicastDst);

	if(destinations.size() == 0)
		click_chatter("!!! No Xcast destination mapping found for multicast address %s", multicastDst.unparse().c_str());

//	// TODO Resize for destinations and pids
//	CastorXcastPkt header(p);
//	size_t oldLength = header.getSize();
//	header.setNDestinations(destinations.size());
//	size_t length = header.getSize() - oldLength;
//	WritablePacket *q = p->push(length);
//	if (!q)
//		return;

	// Write destinations to PKT
	header.setDestinations(destinations.data(), destinations.size());

	Vector<PacketId> pids;
	SValue ackAuth = SValue(header.getAckAuth(), header.getHashSize());
	for(int i = 0; i < destinations.size(); i++) {
		// Generate individual PKT id
		const SymmetricKey* key = _crypto->getSharedKey(destinations[i]);
		if(!key) {
			click_chatter("!!! No key found for multicast destination %s in multicast group %s", destinations[i].unparse().c_str(), multicastDst.unparse().c_str());
			break;
		}
		SValue encAckAuth = _crypto->encrypt(ackAuth, *key);
		if (encAckAuth.size() != sizeof(EACKAuth)) {
			click_chatter("!!! Cannot create ciphertext: Crypto subsystem returned wrong ciphertext length.");
			break;
		}
		SValue pid = _crypto->hash(encAckAuth);
		header.setPid((PacketId&) *pid.begin(), i);
	}

	// Set local node as single forwarder
	header.setNNextHops(1);
	header.setNextHopAssign(destinations.size(), 0);
	header.setNextHop(header.getSource(), 0);

	q->set_dst_ip_anno(header.getSource()); // Fix DST_ANNO

	header.print(true);

	output(0).push(q);

}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorXcastSetDestinations)
