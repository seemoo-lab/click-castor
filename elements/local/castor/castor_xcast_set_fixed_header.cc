#include <click/config.h>
#include <click/confparse.hh>

#include "castor_xcast_set_fixed_header.hh"
#include "castor_xcast.hh"

CLICK_DECLS

CastorXcastSetFixedHeader::CastorXcastSetFixedHeader() {
}

CastorXcastSetFixedHeader::~CastorXcastSetFixedHeader() {
}

int CastorXcastSetFixedHeader::configure(Vector<String> &conf, ErrorHandler *errh) {
     if(cp_va_kparse(conf, this, errh,
        "CastorXcastSetHeader", cpkP+cpkM, cpElementCast, "CastorFlowStub", &cflow,
        cpEnd) < 0)
    	 return -1;
     return 0;
}

void CastorXcastSetFixedHeader::push(int, Packet *p) {

	// Extract source and destination from packet
	IPAddress src = p->ip_header()->ip_src.s_addr;
	IPAddress dst = p->ip_header()->ip_dst.s_addr;

	// Add Space for the new Header
	CastorXcastPkt pkt = CastorXcastPkt::initialize(p);
	pkt.setType(CastorType::XCAST_PKT);
	pkt.setHashSize(sizeof(Hash));
	pkt.setNFlowAuthElements(CASTOR_FLOWAUTH_ELEM);

	pkt.setContentType(p->ip_header()->ip_p);
	pkt.setSource(src);
	pkt.setMulticastGroup(dst);

	// Access the flow settings
	PacketLabel label = cflow->getPacketLabel(src, dst);
	pkt.setFlowId(label.flow_id);
	pkt.setFlowAuth(label.flow_auth);
	pkt.setKPkt(label.packet_number);
	pkt.setAckAuth(label.ack_auth);

	pkt.setNDestinations(0);
	pkt.setNNextHops(0);

	output(0).push(pkt.getPacket());

}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorXcastSetFixedHeader)
