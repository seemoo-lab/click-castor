#include <click/config.h>
#include <click/args.hh>

#include "castor_xcast_set_fixed_header.hh"
#include "castor_xcast.hh"

CLICK_DECLS

int CastorXcastSetFixedHeader::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_mp("FLOW_MAN", ElementCastArg("CastorFlowManager"), flow)
			.complete();
}

Packet* CastorXcastSetFixedHeader::simple_action(Packet *p) {
	// Make space for Xcastor header
	CastorXcastPkt pkt = CastorXcastPkt::initialize(p);

	// Extract source and destination from wrapped IP packet
	NodeId src(reinterpret_cast<const uint8_t*>(&(pkt.getPacket()->ip_header()->ip_src)));
	NodeId dst(reinterpret_cast<const uint8_t*>(&(pkt.getPacket()->ip_header()->ip_dst)));

	pkt.content_type() = pkt.getPacket()->ip_header()->ip_p;
	pkt.src() = src;
	pkt.dst_group() = dst;

	// Access the flow settings
	PacketLabel label = flow->getPacketLabel(src, dst);
	pkt.fid() = label.fid;
	pkt.flow_auth() = label.fauth;
	pkt.kpkt() = htons(label.num);
	pkt.pkt_auth() = label.aauth;

	return pkt.getPacket();
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorXcastSetFixedHeader)
