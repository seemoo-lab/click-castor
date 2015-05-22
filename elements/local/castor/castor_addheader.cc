#include <click/config.h>
#include <click/confparse.hh>

#include "castor_addheader.hh"

CLICK_DECLS

int CastorAddHeader::configure(Vector<String> &conf, ErrorHandler *errh) {
     return cp_va_kparse(conf, this, errh,
        "CastorAddHeader", cpkP+cpkM, cpElementCast, "CastorFlowStub", &cflow,
        cpEnd);
}

void CastorAddHeader::push(int, Packet *p) {

	// Extract source and destination from packet
	NodeId src = p->ip_header()->ip_src.s_addr;
	NodeId dst = p->ip_header()->ip_dst.s_addr;

	// Add Space for the new Header
	uint32_t length = sizeof(CastorPkt);
	WritablePacket *q = p->push(length);
	if (!q)
		return;

	CastorPkt* header = (CastorPkt*) q->data();
	header->type = CastorType::MERKLE_PKT;
	header->hsize = sizeof(Hash);
	header->fsize = CASTOR_FLOWAUTH_ELEM;
	header->len = sizeof(CastorPkt);
#ifdef DEBUG_HOPCOUNT
	header->hopcount = 0;
#endif
	header->ctype = p->ip_header()->ip_p;
	header->src = src;
	header->dst = dst;

	// Access the flow settings
	PacketLabel label = cflow->getPacketLabel(src, dst);

	header->fid = label.flow_id;
	header->pid = label.packet_id;
	header->kpkt = label.packet_number;
	for (int i = 0; i < CASTOR_FLOWAUTH_ELEM; i++)
		header->fauth[i] = label.flow_auth[i];
	header->pauth = label.ack_auth; // not yet encrypted (!)

	CastorPacket::set_src_ip_anno(p, header->src);

	output(0).push(q);

}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorAddHeader)
