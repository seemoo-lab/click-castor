#include <click/config.h>
#include <click/args.hh>
#include <click/confparse.hh>
#include <click/straccum.hh>
#include "castor_print.hh"
#include "castor.hh"

CLICK_DECLS
CastorPrint::CastorPrint(){}

CastorPrint::~ CastorPrint(){}
int CastorPrint::configure(Vector<String> &conf, ErrorHandler *errh) {
	String label;

	if (Args(conf, this, errh)
		.read_p("LABEL", label)
		.complete() < 0)
			return -1;

	_label = label;
	return 0;
}

void CastorPrint::push(int, Packet *p){

	StringAccum sa;

	sa << "CASTOR: " << _label << " ";
	//sa << "   /------------------------------------------------------------\n";

	uint8_t type = CastorPacket::getType(p);

	if( type == CASTOR_TYPE_PKT ){
		Castor_PKT pkt;
		CastorPacket::getCastorPKTHeader(p, &pkt);
		String sfid = CastorPacket::hexToString(pkt.fid, CASTOR_HASHLENGTH);
		String spid = CastorPacket::hexToString(pkt.pid, CASTOR_HASHLENGTH);
		String seauth = CastorPacket::hexToString(pkt.eauth, CASTOR_HASHLENGTH);

		//sa << "   | Type: \tPKT   Length: " <<  pkt.len << "\n";
		//sa << "   | Flow: \t" << pkt.src << " -> " << pkt.dst << "\n";
		//sa << "   | Flow ID: \t" << sfid << "\n";
		//sa << "   | Pkt ID: \t" << spid << "\n";
		//sa << "   | Enc Auth: \t" << seauth << "\n";
		sa << " PKT: ";
		sa << spid;
	}
	else if( type == CASTOR_TYPE_ACK ){
		Castor_ACK ack;
		CastorPacket::getCastorACKHeader(p, &ack);
		String sauth = CastorPacket::hexToString(ack.auth, CASTOR_HASHLENGTH);

		//sa << "   | Type: \tACK   Length: " <<  ack.len << "\n";
		//sa << "   | Auth: \t" << sauth << "\n";

		sa << " ACK: ";
		sa << sauth;

	}
	else{
		sa << " Unknown Packet type";
	}

	//sa << "   | Rec from: \t" <<  p->dst_ip_anno() << "\n";
	//sa << "   \\------------------------------------------------------------\n";

	sa << " SRC: " << p->dst_ip_anno();
	click_chatter("%s", sa.c_str());

    output(0).push(p);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorPrint)
