#include <click/config.h>
#include <click/confparse.hh>
#include "castor_reset_dst_anno.hh"

CLICK_DECLS

void CastorResetDstAnno::push(int, Packet *p) {
	IPAddress addr = CastorPacket::mac_ip_anno(p);
	p->set_dst_ip_anno(addr);
	output(0).push(p);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorResetDstAnno)
