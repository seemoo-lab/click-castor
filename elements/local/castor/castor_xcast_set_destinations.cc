#include <click/config.h>
#include <click/confparse.hh>

#include "castor_xcast_set_destinations.hh"
#include "castor_xcast.hh"

CLICK_DECLS

CastorXcastSetDestinations::CastorXcastSetDestinations() {
	_crypto = 0;
	_map = 0;
}

int CastorXcastSetDestinations::configure(Vector<String> &conf, ErrorHandler *errh) {
     return cp_va_kparse(conf, this, errh,
    	"CRYPT", cpkP+cpkM, cpElementCast, "Crypto", &_crypto,
        "CastorXcastDestinationMap", cpkP+cpkM, cpElementCast, "CastorXcastDestinationMap", &_map,
        cpEnd);
}

void CastorXcastSetDestinations::push(int, Packet *p) {

	CastorXcastPkt pkt = CastorXcastPkt(p);

	IPAddress multicastDst = pkt.getMulticastGroup();
	const Vector<IPAddress>& destinations = _map->getDestinations(multicastDst);

	if(destinations.size() == 0) {
		click_chatter("!!! No Xcast destination mapping found for multicast address %s, drop packet", multicastDst.unparse().c_str());
		pkt.getPacket()->kill();
	}

	// Write destinations to PKT
	pkt.setDestinations(destinations.data(), destinations.size());

	Vector<PacketId> pids;
	SValue ackAuth(&pkt.getAckAuth()[0], pkt.getHashSize());
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
		pkt.setPid((PacketId&) *pid.begin(), i);
	}

	// Set local node as single forwarder
	pkt.setSingleNextHop(pkt.getSource());

	CastorPacket::set_src_ip_anno(pkt.getPacket(), pkt.getSource());

	output(0).push(pkt.getPacket());

}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorXcastSetDestinations)
