#include <click/config.h>
#include <click/confparse.hh>
#include "castor_removeheader.hh"

CLICK_DECLS

CastorRemoveHeader::CastorRemoveHeader(){
	init();
}

CastorRemoveHeader::~ CastorRemoveHeader(){}

int CastorRemoveHeader::configure(Vector<String> &conf, ErrorHandler *errh) {
     return cp_va_kparse(conf, this, errh,
        cpEnd);
}

void CastorRemoveHeader::init(){
	click_ip ip;
	memset(&ip, 0, sizeof(click_ip));

	//Set default values for the IP header
	ip.ip_v = 4;
	ip.ip_hl = sizeof(click_ip) >> 2;
	ip.ip_ttl = 250;
	ip.ip_tos = 0;

	_iph = ip;
}

/**
 * Removes the Castor Header and restores the original IP Header
 */
void CastorRemoveHeader::push(int, Packet *p){

	//Extract src and destination from the Castor Header
//	Castor_PKT header;
//	CastorPacket::getCastorPKTHeader(p, &header);
//	IPAddress* src = &header.src;
//	IPAddress* dst = &header.dst;
//	uint8_t type  = header.ctype;

	//Remove the Castor Header
	WritablePacket *q = p->uniqueify();
	//q -> pull( sizeof(Castor_PKT) - sizeof(click_ip));
	q -> pull( sizeof(Castor_PKT));

//	//Add a new IP Header
//	click_ip *ip = reinterpret_cast<click_ip *>(q->data());
//	memcpy(ip, &_iph, sizeof(click_ip));
//	ip->ip_src = src->in_addr();
//	ip->ip_dst = dst->in_addr();
//	ip->ip_p = type;
//	ip->ip_len = htons(p->length());

	output(0).push(q);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorRemoveHeader)
