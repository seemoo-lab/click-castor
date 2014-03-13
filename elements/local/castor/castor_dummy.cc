#include <click/config.h>
#include <click/confparse.hh>

#include "castor_dummy.hh"

CLICK_DECLS
Dummy::Dummy(){}

Dummy::~ Dummy(){}

int Dummy::configure(Vector<String> &conf, ErrorHandler *errh) {
    //return cp_va_kparse(conf, this, errh,
	//	"CastorAddHeader", cpkP+cpkM, cpElementCast, cpEnd);
	click_chatter("Dummy called");
	return 0;
}

//void Dummy::push(int, Packet *p){
//
//	//TODO: Implement valiation check here
//
//    output(0).push(p);
//
//    //TODO: If validation fails, push to output 1 to be discarded
//}

CLICK_ENDDECLS
EXPORT_ELEMENT(Dummy)
