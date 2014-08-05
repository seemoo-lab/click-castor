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
	unsigned int maxGroupSize;

     int result = cp_va_kparse(conf, this, errh,
        "CastorXcastSetHeader", cpkP+cpkM, cpElementCast, "CastorFlowStub", &cflow,
        "MaxGroupSize", cpkP+cpkM, cpUnsigned, &maxGroupSize, // TODO for simpler implementation -> no need to resize packet in subsequent elements, but transmits potentially larger packets
        cpEnd);
     varSpace = maxGroupSize * (sizeof(IPAddress) + sizeof(PacketId)) + // Space for 'maxGroupSize' destinations
     	 	 	maxGroupSize * (sizeof(IPAddress) + sizeof(uint8_t));   // Space for one next hop per destination

     if(result < 0)
    	 return -1;
     return 0;
}

void CastorXcastSetFixedHeader::push(int, Packet *p) {

	// Extract source and destination from packet
	IPAddress src = p->ip_header()->ip_src.s_addr;
	IPAddress dst = p->ip_header()->ip_dst.s_addr;

	// Add Space for the new Header
	size_t length = CastorXcastPkt::getFixedSize();
	WritablePacket *q = p->push(length + varSpace);
	if (!q)
		return;

	CastorXcastPkt header = CastorXcastPkt(q);
	header.setType(CastorType::XCAST_PKT);
	header.setHashSize(sizeof(Hash));
	header.setNFlowAuthElements(CASTOR_FLOWAUTH_ELEM);
	header.setLength(length);

	header.setContentType(p->ip_header()->ip_p);
	header.setSource(src);
	header.setMulticastGroup(dst);

	// Access the flow settings
	PacketLabel label = cflow->getPacketLabel(src, dst);
	header.setFlowId(label.flow_id);
	header.setFlowAuth(label.flow_auth);
	header.setKPkt(label.packet_number);
	header.setAckAuth(label.ack_auth);

	header.setNDestinations(0);
	header.setNNextHops(0);

	output(0).push(q);

}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorXcastSetFixedHeader)
