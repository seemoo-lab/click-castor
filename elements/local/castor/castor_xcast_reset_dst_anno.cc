#include <click/config.h>
#include <click/args.hh>
#include <click/confparse.hh>
#include "castor_xcast_reset_dst_anno.hh"

CLICK_DECLS

CastorXcastResetDstAnno::CastorXcastResetDstAnno() {
}

CastorXcastResetDstAnno::~CastorXcastResetDstAnno() {
}

int CastorXcastResetDstAnno::configure(Vector<String>& conf, ErrorHandler* errh) {
	if(Args(conf, errh)
			.read_mp("PROMISC", _promisc)
			.complete() < 0)
		return -1;
	return 0;
}

void CastorXcastResetDstAnno::push(int, Packet *p){

	if(_promisc) {
		IPAddress addr = CastorPacket::mac_ip_anno(p);
		p->set_dst_ip_anno(addr);
	}

	output(0).push(p);

}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorXcastResetDstAnno)
