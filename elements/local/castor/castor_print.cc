#include <click/config.h>
#include <click/args.hh>
#include <click/straccum.hh>
#include <click/timestamp.hh>
#include "castor_print.hh"
#include "castor.hh"
#include "castor_anno.hh"

CLICK_DECLS

int CastorPrint::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
		.read_mp("LABEL", label)
		.read_mp("ID", myId)
		.read_or_set_p("VERBOSE", verbose, false)
		.read_or_set("DELAY", delay, false)
		.complete();
}

Packet* CastorPrint::simple_action(Packet *p) {
	StringAccum sa;
	sa << "[" << Timestamp::now() << "@" << myId;
	if (delay)
		sa << " (" << (Timestamp::now() - p->timestamp_anno()).usec() << ")";
	sa << "] " << label << " ";

	uint8_t type = CastorPacket::getType(p);

	if (type == CastorType::PKT) {

		CastorPkt& pkt = (CastorPkt&) *p->data();
		if(verbose) {
			sa << "\n";
			sa << "   | From: \t" << CastorAnno::src_id_anno(p) << "\n";
			sa << "   | To: \t" << CastorAnno::dst_id_anno(p) << "\n";
			sa << "   | Type: \tPKT (header " <<  pkt.header_len() << " / payload " << pkt.payload_len() << " bytes)\n";
			sa << "   | Flow: \t" << pkt.src << " -> " << pkt.dst << "\n";
			sa << "   | Flow ID: \t" << pkt.fid.str() << "\n";
			sa << "   | PKT ID: \t" << pkt.pid.str() << " (" << (ntohs(pkt.kpkt) + 1) << "/" << (1 << pkt.fsize) << ")\n";
			if (pkt.syn)
			sa << "   | Nonce: \t" << pkt.n()->str() << "\n";
		} else {
			sa << "PKT (from " << CastorAnno::src_id_anno(p) << " to " << CastorAnno::dst_id_anno(p) << ", flow " << pkt.src << " -> " << pkt.dst << "): " << pkt.pid.str() << " (" << (ntohs(pkt.kpkt) + 1) << "/" << (1 << pkt.fsize) << ")";
		}

	} else if (type == CastorType::ACK){

		// CastorAck and CastorXcastAck are identical
		CastorAck& ack = (CastorAck&) *p->data();
		if(verbose) {
			sa << "\n";
			sa << "   | From: \t" << CastorAnno::src_id_anno(p) << "\n";
			sa << "   | To: \t" << CastorAnno::dst_id_anno(p) << "\n";
			sa << "   | Type: \tACK  (" <<  ack.len << " bytes)\n";
			sa << "   | ACK Auth: \t" << ack.auth.str() << "\n";
#ifdef DEBUG_ACK_SRCDST
			sa << "   | Flow: \t" << ack.src << " -> " << ack.dst << "\n";
#endif
		} else {
			sa << "ACK (from " << CastorAnno::src_id_anno(p) << " to " << CastorAnno::dst_id_anno(p) << "): " << ack.auth.str();
		}

	} else {
		sa << "UNKNOWN (from " << CastorAnno::src_id_anno(p) << " to " << CastorAnno::dst_id_anno(p) << ")";
	}

	click_chatter("%s", sa.c_str());

	return p;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorPrint)
