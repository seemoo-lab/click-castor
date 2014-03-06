#include <click/config.h>
#include <click/confparse.hh>
#include <click/packet.hh>
#include "castor_createACK.hh"

CLICK_DECLS
CastorCreateACK::CastorCreateACK(){}

CastorCreateACK::~CastorCreateACK(){}

int CastorCreateACK::configure(Vector<String> &conf, ErrorHandler *errh)
{
	int res = cp_va_kparse(conf, this, errh,
		"CRYPT", cpkP+cpkM, cpElementCast, "Crypto", &_crypto,
		cpEnd);
	if(res < 0) return res;
	return 0;
}

/**
 * Create an ACK for incoming packet. The original packet is pushed to output 0, the ACK is pushed on output 1
 */
void CastorCreateACK::push(int, Packet *p){

	click_chatter("Creating an ACK Packet");

	//Extract the Encrypted Authenticator from the packet
    Castor_PKT* pkt = (Castor_PKT*) p->data();

	//Decrypt the authenticator
    Private_Key* sk = _crypto->getPrivateKey(pkt->dst);
    SValue eauth = SValue(pkt->eauth, CASTOR_ENCLENGTH);
	SValue aauth = _crypto->decrypt(&eauth, sk);

	//Generate a new ACK packet
	Castor_ACK ack;
	ack.type 	= CASTOR_TYPE_ACK;
    ack.hsize 	= CASTOR_HASHLENGTH;
	ack.len 	= sizeof(Castor_ACK);

	memcpy(&ack.auth, aauth.begin(), CASTOR_HASHLENGTH);

	 WritablePacket* q = Packet::make(&ack,sizeof(Castor_ACK));
	 q->set_dst_ip_anno(IPAddress("255.255.255.255"));

	output(0).push(p);
	output(1).push(q);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorCreateACK)
