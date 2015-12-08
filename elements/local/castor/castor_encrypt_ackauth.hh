#ifndef CLICK_CASTOR_ENCRYPT_ACKAUTH_HH
#define CLICK_CASTOR_ENCRYPT_ACKAUTH_HH

#include <click/element.hh>
#include "crypto/crypto.hh"

CLICK_DECLS

class CastorEncryptAckAuth : public Element {
public:
	const char *class_name() const	{ return "CastorEncryptAckAuth"; }
	const char *port_count() const	{ return PORTS_1_1X2; }
	const char *processing() const	{ return PROCESSING_A_AH; }
	int configure(Vector<String>&, ErrorHandler*);

	Packet* simple_action(Packet *);
private:
	Crypto* crypto;
};

CLICK_ENDDECLS

#endif
