#include <click/config.h>
#include <click/args.hh>
#include <click/confparse.hh>
#include "castor_encryptackauth_dummy.hh"

CLICK_DECLS
CastorEncryptACKAuthDummy::CastorEncryptACKAuthDummy(){}

CastorEncryptACKAuthDummy::~ CastorEncryptACKAuthDummy(){}

int CastorEncryptACKAuthDummy::configure(Vector<String> &conf, ErrorHandler *errh)
{
	int res = cp_va_kparse(conf, this, errh,
		"CRYPT", cpkP+cpkM, cpElementCast, "Crypto", &_crypto,
		"ADDR", cpkP+cpkM, cpIPAddress, &_myIP,
		cpEnd);
	if(res < 0) return res;
	return 0;
}


void CastorEncryptACKAuthDummy::push(int, Packet *p){

	output(0).push(p);

}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorEncryptACKAuthDummy)
