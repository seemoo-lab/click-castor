#include <click/config.h>
#include <click/args.hh>
#include "castor_add_header.hh"

CLICK_DECLS

int CastorAddHeader::configure(Vector<String> &conf, ErrorHandler *errh) {
     return Args(conf, this, errh)
    		 .read_mp("FLOW_MANAGER", ElementCastArg("CastorFlowManager"), flow)
			 .complete();
}

Packet* CastorAddHeader::simple_action(Packet *p) {

	// Extract source and destination from packet
	NodeId src(reinterpret_cast<const uint8_t*>(&(p->ip_header()->ip_src)));
	NodeId dst(reinterpret_cast<const uint8_t*>(&(p->ip_header()->ip_dst)));

	// Access the flow settings
	PacketLabel label = flow->getPacketLabel(src, dst);
	unsigned int fasize = 0;

	// Add Space for the new Header
	unsigned int length = sizeof(CastorPkt) + fasize * sizeof(Hash);
	WritablePacket *q = p->push(length);
	if (!q)
		return 0;

	CastorPkt* header = (CastorPkt*) q->data();
	header->type = CastorType::MERKLE_PKT;
	header->hsize = sizeof(Hash);
	header->fsize = label.size;
	header->arq = 0;
	header->len = htons(length);
#ifdef DEBUG_HOPCOUNT
	header->hopcount = 0;
#endif
	header->fasize = fasize;
	header->src = src;
	header->dst = dst;

	header->fid = label.fid;
	header->pid = label.pid;
	header->kpkt = htons(label.num);
	header->pauth = label.aauth; // not yet encrypted (!)
	header->icv = ICV();

	return q;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorAddHeader)
