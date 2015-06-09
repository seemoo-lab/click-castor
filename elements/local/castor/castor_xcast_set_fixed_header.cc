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

	pkt.setType(CastorType::XCAST_PKT);
	pkt.setHashSize(sizeof(Hash));
	pkt.setNFlowAuthElements(CASTOR_FLOWAUTH_ELEM);

	// Extract source and destination from wrapped IP packet
	NodeId src = pkt.getPacket()->ip_header()->ip_src.s_addr;
	NodeId dst = pkt.getPacket()->ip_header()->ip_dst.s_addr;

	pkt.setContentType(pkt.getPacket()->ip_header()->ip_p);
	pkt.setSource(src);
	pkt.setMulticastGroup(dst);

	// Access the flow settings
	PacketLabel label = flow->getPacketLabel(src, dst);
	pkt.setFlowId(label.fid);
	pkt.setFlowAuth(label.fauth);
	pkt.setKPkt(label.num);
	pkt.setAckAuth(label.aauth);

	output(0).push(pkt.getPacket());
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorXcastSetFixedHeader)
