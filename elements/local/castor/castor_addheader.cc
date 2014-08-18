#include <click/config.h>
#include <click/confparse.hh>

#include "castor_addheader.hh"

CLICK_DECLS

CastorAddHeader::CastorAddHeader() {
}

CastorAddHeader::~CastorAddHeader() {
}

int CastorAddHeader::configure(Vector<String> &conf, ErrorHandler *errh) {
     return cp_va_kparse(conf, this, errh,
        "CastorAddHeader", cpkP+cpkM, cpElementCast, "CastorFlowStub", &cflow,
        cpEnd);
}

void CastorAddHeader::push(int, Packet *p) {

	// Extract source and destination from packet
	IPAddress src = p->ip_header()->ip_src.s_addr;
	IPAddress dst = p->ip_header()->ip_dst.s_addr;

	// Add Space for the new Header
	uint32_t length = sizeof(Castor_PKT);
	WritablePacket *q = p->push(length);
	if (!q)
		return;

	Castor_PKT* header = (Castor_PKT*) q->data();
	header->type = CastorType::MERKLE_PKT;
	header->hsize = sizeof(Hash);
	header->fsize = CASTOR_FLOWAUTH_ELEM;
	header->len = sizeof(Castor_PKT);

	header->ctype = p->ip_header()->ip_p;
	header->src = src;
	header->dst = dst;

	//Acces the flow settings
	PacketLabel label = cflow->getPacketLabel(src, dst);

	memcpy(&header->fid, &label.flow_id, sizeof(FlowId));
	memcpy(&header->pid, &label.packet_id, sizeof(PacketId));
	header->packet_num = label.packet_number;
	for (int i = 0; i < CASTOR_FLOWAUTH_ELEM; i++)
		memcpy(&header->fauth[i], &label.flow_auth[i], sizeof(Hash));
	if (sizeof(Hash) > sizeof(EACKAuth)) {
		click_chatter("[Warning] Copying ACKAuth: Hash length is larger than ciphertext length, loosing entropy.");
		memcpy(&header->eauth, &label.ack_auth, sizeof(EACKAuth));
	} else {
		memcpy(&header->eauth, &label.ack_auth, sizeof(Hash));
	}

	CastorPacket::set_src_ip_anno(p, header->src);

	output(0).push(q);

}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorAddHeader)
