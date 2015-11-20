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
	const char *class_name() const { return "CastorDecryptAckAuth"; }
	const char *port_count() const { return "1/1-2"; }
	const char *processing() const { return PUSH; }
	int configure(Vector<String>&, ErrorHandler*);

	void push(int, Packet*);
private:
	Crypto* crypto;

};

CLICK_ENDDECLS

#endif
