/*
 * castor_validateACK.cc
 *
 *  Created on: Jun 23, 2014
 *      Author: milan
 */
#include <click/config.h>
#include <click/confparse.hh>
#include "castor_validateACK.hh"

CLICK_DECLS

CastorValidateACK::CastorValidateACK() {
}

CastorValidateACK::~CastorValidateACK() {
}

int CastorValidateACK::configure(Vector<String>& conf, ErrorHandler* errh) {
    return cp_va_kparse(conf, this, errh,
		"CastorHistory", cpkP+cpkM, cpElementCast, "CastorHistory", &_history,
        cpEnd);
}

void CastorValidateACK::push(int, Packet* p) {

	if(_history->ValidateACK(p)) {
	    output(0).push(p);
	} else {
	    output(1).push(p); // -> discard
	}

}


CLICK_ENDDECLS
EXPORT_ELEMENT(CastorValidateACK)
