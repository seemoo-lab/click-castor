#include <click/config.h>
#include <click/confparse.hh>

#include "castor_checkheader.hh"

CLICK_DECLS
CastorCheckHeader::CastorCheckHeader(){}

CastorCheckHeader::~ CastorCheckHeader(){}

int CastorCheckHeader::configure(Vector<String> &conf, ErrorHandler *errh) {
    //return cp_va_kparse(conf, this, errh,
	//	"CastorAddHeader", cpkP+cpkM, cpElementCast, cpEnd);
	return 0;
}

void CastorCheckHeader::push(int, Packet *p){

    uint8_t type = getCastorType(p->data());

    if(type == CASTOR_TYPE_PKT){
        //click_chatter("Indicated Castor Packet");
        output(0).push(p);
    } else if (type == CASTOR_TYPE_ACK){
        //click_chatter("Indicated Castor ACK");
        output(1).push(p);
    } else {
        //click_chatter("Indicated non Castor Packet");
        output(2).push(p);
    }

}

uint8_t CastorCheckHeader::getCastorType(const unsigned char * data){
    uint8_t type;
    memcpy(&type, data, sizeof(type));
    type = type & 0xF0;
    return type;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorCheckHeader)
