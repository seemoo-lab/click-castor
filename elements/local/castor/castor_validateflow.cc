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

	//TODO: Implement valiation check here

    output(0).push(p);

    //TODO: If validation fails, push to output 1 to be discarded
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorValidateFlow)
