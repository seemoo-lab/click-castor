#include <click/config.h>
#include <click/args.hh>
#include <click/confparse.hh>
#include <click/straccum.hh>
#include "castor_dstclassifier.hh"

CLICK_DECLS
CastorDstClassifier::CastorDstClassifier(){
}

CastorDstClassifier::~ CastorDstClassifier(){}

//int CastorDstClassifier::configure(Vector<String> &conf, ErrorHandler *errh) {
//     return cp_va_kparse(conf, this, errh,
//        "CastorDstClassifier", cpkP+cpkM, cpElementCast, "String", &_conf,
//        cpEnd);
//}

int CastorDstClassifier::configure(Vector<String> &conf, ErrorHandler *errh) {
	IPAddress dst;
	   if (Args(conf, this, errh)
			   .read_mp("DST", dst)
			   .complete() < 0)
		return -1;

	   _my_addr = dst;
}


void CastorDstClassifier::push(int, Packet *p){

	IPAddress dest 	= getCastorPktHeader(p->data()).dst;

	if(_my_addr == dest){
		output(0).push(p);
	}
	else{
		output(1).push(p);
	}

}

Castor_PKT CastorDstClassifier::getCastorPktHeader(const unsigned char * data){
    // Create a new header object
    Castor_PKT header;

    // Copy the header from packet
    memcpy(&header, data, sizeof(Castor_PKT));

    return header;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorDstClassifier)
