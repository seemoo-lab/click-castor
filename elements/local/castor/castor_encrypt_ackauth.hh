#ifndef CLICK_CASTOR_ENCRYPT_ACKAUTH_HH
#define CLICK_CASTOR_ENCRYPT_ACKAUTH_HH

#include <click/element.hh>
#include "castor.hh"
#include "crypto/crypto.hh"

CLICK_DECLS

class CastorEncryptAckAuth : public Element {
public:
	CastorEncryptAckAuth() : crypto(NULL) {}

	const char *class_name() const	{ return "CastorEncryptAckAuth"; }
	const char *port_count() const	{ return PORTS_1_1; }
	const char *processing() const	{ return PUSH; }
	int configure(Vector<String>&, ErrorHandler*);

	void push(int, Packet *);

private:
	Crypto* crypto;
};

CLICK_ENDDECLS

#endif
