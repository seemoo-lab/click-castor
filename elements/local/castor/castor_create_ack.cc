#include <click/config.h>
#include <click/args.hh>
#include "castor_create_ack.hh"
#include "castor.hh"
#include "castor_anno.hh"

CLICK_DECLS

int CastorCreateAck::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
		.read_mp("FlowTable", ElementCastArg("CastorFlowTable"), flowtable)
		.complete();
}

Packet* CastorCreateAck::simple_action(Packet* p) {
	const CastorPkt& pkt = *reinterpret_cast<const CastorPkt*>(p->data());

	CastorAck ack;
	ack.type = CastorType::MERKLE_ACK;
	ack.hsize = sizeof(AckAuth);
	ack.len = htons(sizeof(CastorAck));
	ack.fid  = pkt.fid;
	ack.auth = flowtable->get(pkt.fid).aauths[ntohs(pkt.kpkt)];

	WritablePacket* q = Packet::make(&ack, sizeof(CastorAck));
	CastorAnno::dst_id_anno(q) = CastorAnno::src_id_anno(p); // Set DST_ANNO to source of PKT
	CastorAnno::hop_id_anno(q) = CastorAnno::dst_id_anno(q);

	output(1).push(q); // ACK -> output 1
	return p;          // PKT -> output 0
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorCreateAck)
