#include <click/config.h>
#include <click/args.hh>
#include <click/confparse.hh>
#include "castor_encryptackauth.hh"
#include "crypto.hh"
#include <botan/data_src.h>
#include <click/straccum.hh>




CLICK_DECLS
CastorEncryptACKAuth::CastorEncryptACKAuth(){}

CastorEncryptACKAuth::~ CastorEncryptACKAuth(){}

int CastorEncryptACKAuth::configure(Vector<String> &conf, ErrorHandler *errh)
{
	int res = cp_va_kparse(conf, this, errh,
//		"SAM", cpkP+cpkM, cpElementCast, "SAManagement", &_sam,
		"CRYPT", cpkP+cpkM, cpElementCast, "Crypto", &_crypto,
		"ADDR", cpkP+cpkM, cpIPAddress, &_myIP,
		cpEnd);
	if(res < 0) return res;
	return 0;
}


void CastorEncryptACKAuth::push(int, Packet *p){

	if(CastorPacket::getType(p) != CASTOR_TYPE_PKT){
		//Error, not a Castor Packet
		click_chatter("Error, CastorEncryptACKAuth can only handle CASTOR Packets");
		return;
	}

	WritablePacket* q = p->uniqueify();
	Castor_PKT* header = (Castor_PKT*) q->data();
	SValue auth(header->eauth, CASTOR_HASHLENGTH);

	PublicKey* pk = _crypto->getPublicKey(header->dst);
	if(!pk){
		click_chatter("Could not find public key, for destination");
		return;
	}

	SValue enc = _crypto->encrypt(&auth, pk);

	if(enc.size() != CASTOR_ENCLENGTH){
		click_chatter("Cannot create propper encryption, Crypto subsystem returned wrong cipher length");
		return;
	}

	memcpy(header->eauth, enc.begin(), CASTOR_ENCLENGTH);

	//click_chatter("ACK Authenticator encrypted");

	output(0).push(q);

}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorEncryptACKAuth)

