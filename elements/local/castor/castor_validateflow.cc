#include <click/config.h>
#include <click/confparse.hh>

#include "castor_validateflow.hh"

CLICK_DECLS
CastorValidateFlow::CastorValidateFlow(){}

CastorValidateFlow::~ CastorValidateFlow(){}

//int CastorValidateFlow::configure(Vector<String> &conf, ErrorHandler *errh) {
//    //return cp_va_kparse(conf, this, errh,
//	//	"CastorAddHeader", cpkP+cpkM, cpElementCast, cpEnd);
//	return 0;
//}

void CastorValidateFlow::push(int, Packet *p){

	// TODO: Implement validation check here
	bool isPacketValid = true;

	if(isPacketValid)
		output(0).push(p);
	else
		output(1).push(p); // Invalid; -> discard

}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorValidateFlow)
