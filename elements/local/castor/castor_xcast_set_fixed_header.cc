#include <click/config.h>
#include <click/confparse.hh>

#include "castor_xcast_set_fixed_header.hh"
#include "castor_xcast.hh"

CLICK_DECLS

int CastorXcastSetFixedHeader::configure(Vector<String> &conf, ErrorHandler *errh) {
     return cp_va_kparse(conf, this, errh,
        "CastorXcastSetHeader", cpkP+cpkM, cpElementCast, "CastorFlowManager", &flow,
        cpEnd);
}

void CastorXcastSetFixedHeader::push(int, Packet *p) {
	// Make space for Xcastor header
	CastorXcastPkt pkt = CastorXcastPkt::initialize(p);

	pkt.type() = CastorType::XCAST_PKT;
	pkt.hash_size() = sizeof(Hash);
	pkt.flow_size() = CASTOR_FLOWAUTH_ELEM;

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
	pkt.kpkt() = label.num;
	pkt.pkt_auth() = label.aauth;

	output(0).push(pkt.getPacket());
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorXcastSetFixedHeader)
