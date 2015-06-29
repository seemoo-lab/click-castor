#ifndef CLICK_CASTOR_DECRYPT_ACKAUTH_HH
#define CLICK_CASTOR_DECRYPT_ACKAUTH_HH

#include <click/element.hh>
#include "crypto/crypto.hh"

CLICK_DECLS

/**
 * Decrypt the encrypted ACK authenticator field of the incoming packet.
 */
class CastorDecryptAckAuth : public Element {

public:
	CastorDecryptAckAuth() : crypto(NULL) {}

	const char *class_name() const { return "CastorDecryptAckAuth"; }
	const char *port_count() const { return PORTS_1_1; }
	const char *processing() const { return PUSH; }
	int configure(Vector<String>&, ErrorHandler*);

	void push(int, Packet*);

private:
	Crypto* crypto;

};

CLICK_ENDDECLS

#endif
