#include <click/config.h>
#include <click/confparse.hh>
#include "castor_checkduplicate.hh"

CLICK_DECLS

CastorCheckDuplicate::CastorCheckDuplicate() {
}

CastorCheckDuplicate::~CastorCheckDuplicate() {
}

int CastorCheckDuplicate::configure(Vector<String> &conf, ErrorHandler *errh) {
     return cp_va_kparse(conf, this, errh,
        "CastorHistory", cpkP+cpkM, cpElementCast, "CastorHistory", &_history,
        cpEnd);
}

void CastorCheckDuplicate::push(int, Packet *p){
	if(_history->checkDuplicate(p)){
		// We already transmitted this packet, discard
		//click_chatter("Duplicated Packet detected, discarding ...");
		p->kill();
	}
	else{
		output(0).push(p);
	}
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorCheckDuplicate)
