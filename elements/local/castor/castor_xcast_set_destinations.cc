#include <click/config.h>
#include <click/args.hh>
#include "castor_xcast_set_destinations.hh"
#include "castor.hh"
#include "castor_xcast.hh"

CLICK_DECLS

int CastorXcastSetDestinations::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_mp("Crypto", ElementCastArg("Crypto"), crypto)
			.read_mp("MAP", ElementCastArg("CastorXcastDestinationMap"), map)
			.read_mp("ID", my_id)
			.complete();
}

void CastorXcastSetDestinations::push(int, Packet *p) {

	CastorXcastPkt pkt = CastorXcastPkt(p);

	GroupId multicastDst = pkt.getMulticastGroup();
	const Vector<NodeId>& destinations = map->get(multicastDst);

	if(destinations.size() == 0) {
		click_chatter("!!! No Xcast destination mapping found for multicast address %s, drop packet", multicastDst.unparse().c_str());
		pkt.getPacket()->kill();
		return;
	}

	// Write destinations to PKT
	pkt.setDestinations(destinations.data(), destinations.size());

	Vector<PacketId> pids;
	SValue ackAuth = crypto->convert(pkt.getPktAuth());
	for(int i = 0; i < destinations.size(); i++) {
		// Generate individual PKT id
		const SymmetricKey* key = crypto->getSharedKey(destinations[i]);
		if(!key) {
			click_chatter("!!! No key found for multicast destination %s in multicast group %s", destinations[i].unparse().c_str(), multicastDst.unparse().c_str());
			break;
		}
		SValue encAckAuth = crypto->encrypt(ackAuth, *key);
		if (encAckAuth.size() != sizeof(PktAuth)) {
			click_chatter("!!! Cannot create ciphertext: Crypto subsystem returned wrong ciphertext length.");
			break;
		}
		PacketId pid = crypto->hashConvert(encAckAuth);
		pkt.setPid(pid, i);
	}

	// Set local node as single forwarder
	pkt.setSingleNextHop(my_id);

	output(0).push(pkt.getPacket());

}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorXcastSetDestinations)
