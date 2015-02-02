#ifndef CLICK_CASTOR_AUTHENTICATE_ACK_HH
#define CLICK_CASTOR_AUTHENTICATE_ACK_HH

#include <click/element.hh>
#include "castor.hh"
#include "crypto.hh"
#include "castor_history.hh"

CLICK_DECLS

class CastorAuthenticateAck: public Element {
public:
	CastorAuthenticateAck() : crypto(0), history(0), version(1) {}

	const char *class_name() const { return "CastorAuthenticateAck"; }
	const char *port_count() const { return "1/5-6"; }
	const char *processing() const { return PUSH; }
	int configure(Vector<String>&, ErrorHandler*);

	void push(int, Packet *);
private:
	Crypto* crypto;
	CastorHistory* history;
	int version;
};

CLICK_ENDDECLS

#endif
