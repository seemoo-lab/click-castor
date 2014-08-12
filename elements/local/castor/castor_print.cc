#include <click/config.h>
#include <click/args.hh>
#include <click/confparse.hh>
#include <click/straccum.hh>
#include "castor_print.hh"
#include "castor_xcast.hh"
#include <click/timestamp.hh>

CLICK_DECLS

CastorPrint::CastorPrint() {
}

CastorPrint::~CastorPrint() {
}

int CastorPrint::configure(Vector<String> &conf, ErrorHandler *errh) {
	_fullpkt = false;

	if (Args(conf, this, errh)
		.read_mp("LABEL", _label)
		.read_mp("ADDR", _address)
		.read_p("FULL", _fullpkt)
		.complete() < 0)
			return -1;
	return 0;
}

void CastorPrint::push(int, Packet *p){

	StringAccum sa;
	sa << "[" << Timestamp::now() << "@" << _address << "] " << _label << " ";

	uint8_t type = CastorPacket::getType(p);

	if( type == CastorType::PKT ){

		if(CastorPacket::isXcast(p)) {
			sa << (_fullpkt ? "\n" : "") << CastorXcastPkt(p).toString(_fullpkt).c_str();
		} else {
			Castor_PKT& pkt = (Castor_PKT&) *p->data();
			String spid = CastorPacket::hexToString(pkt.pid, sizeof(PacketId));
			if(_fullpkt) {
				String sfid = CastorPacket::hexToString(pkt.fid, sizeof(FlowId));
				String seauth = CastorPacket::hexToString(pkt.eauth, sizeof(EACKAuth));
				sa << "\n";
				sa << "   | From: \t" << p->dst_ip_anno() << "\n";
				sa << "   | Type: \tPKT (header " <<  pkt.len << " / payload " << (p->length() - pkt.len) << " bytes)\n";
				sa << "   | Flow: \t" << pkt.src << " -> " << pkt.dst << "\n";
				sa << "   | Flow ID: \t" << sfid << "\n";
				sa << "   | Pkt ID: \t" << spid << " (" << (pkt.packet_num + 1) << "/" << (1 << pkt.fsize) << ")\n";
				sa << "   | Enc Auth: \t" << seauth;
			} else {
				sa << "PKT (from " << p->dst_ip_anno() << ", flow " << pkt.src << " -> " << pkt.dst << ")";
			}
		}

	} else if( type == CastorType::ACK ){

		// TODO handle XcastAck

		Castor_ACK& ack = (Castor_ACK&) *p->data();
		String sauth = CastorPacket::hexToString(ack.auth, sizeof(ACKAuth));
		if(_fullpkt) {
			sa << "\n";
			sa << "   | From: \t" << p->dst_ip_anno() << "\n";
			sa << "   | Type: \tACK  (" <<  ack.len << " bytes)\n";
			sa << "   | Auth: \t" << sauth << "\n";
		} else {
			sa << "ACK (from " << p->dst_ip_anno() << "): " << sauth;
		}

	} else {

		sa << "Unknown type (from " << p->dst_ip_anno() << ")";

	}

	click_chatter("%s", sa.c_str());

    output(0).push(p);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorPrint)
