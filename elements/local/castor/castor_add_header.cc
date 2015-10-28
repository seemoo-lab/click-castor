#include <click/config.h>
#include <click/confparse.hh>

#include "castor_add_header.hh"

CLICK_DECLS

int CastorAddHeader::configure(Vector<String> &conf, ErrorHandler *errh) {
     return cp_va_kparse(conf, this, errh,
        "CastorAddHeader", cpkP+cpkM, cpElementCast, "CastorFlowManager", &flow,
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
	header->len = htons(sizeof(CastorPkt));
#ifdef DEBUG_HOPCOUNT
	header->hopcount = 0;
#endif
	header->ctype = p->ip_header()->ip_p;
	header->src = src;
	header->dst = dst;

	// Access the flow settings
	PacketLabel label = flow->getPacketLabel(src, dst);

	header->fid = label.fid;
	header->pid = label.pid;
	header->kpkt = htons(label.num);
	for (int i = 0; i < CASTOR_FLOWAUTH_ELEM; i++)
		header->fauth[i] = label.fauth[i];
	header->pauth = label.aauth; // not yet encrypted (!)

	CastorPacket::set_src_ip_anno(p, header->src);

	output(0).push(q);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorAddHeader)
