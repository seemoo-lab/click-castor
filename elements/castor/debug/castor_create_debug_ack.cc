#include <click/config.h>
#include <click/args.hh>
#include "castor_create_debug_ack.hh"
#include "../castor.hh"
#include "../castor_anno.hh"

CLICK_DECLS

int CastorCreateDebugAck::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_mp("FlowTable", ElementCastArg("CastorFlowTable"), flowtable)
			.complete();
}

Packet* CastorCreateDebugAck::simple_action(Packet* p) {
	const CastorPkt& pkt = *reinterpret_cast<const CastorPkt*>(p->data());

	CastorAck ack;
	ack.type = CastorType::ACK;
	ack.len = htons(sizeof(CastorAck));
	ack.fid = pkt.fid;
	ack.auth = pkt.pid;
	pkt.dbg() ? ack.set_dbg() : ack.unset_dbg();
	pkt.insp() ? ack.set_insp() : ack.unset_insp();

	// Creats new CastorAck packet
	WritablePacket* q = Packet::make(&ack, sizeof(CastorAck));
	CastorAnno::dst_id_anno(q) = CastorAnno::src_id_anno(p);
	CastorAnno::hop_id_anno(q) = CastorAnno::dst_id_anno(q);

	output(1).push(q);	//ACK
	return p;		//PKT
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorCreateDebugAck)
