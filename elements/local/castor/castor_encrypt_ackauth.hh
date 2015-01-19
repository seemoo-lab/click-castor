#ifndef CLICK_CASTOR_ENCRYPT_ACKAUTH_HH
#define CLICK_CASTOR_ENCRYPT_ACKAUTH_HH

#include <click/element.hh>
#include "castor.hh"
#include "crypto.hh"

CLICK_DECLS

class CastorEncryptACKAuth : public Element {
	public:
		CastorEncryptACKAuth();

		const char *class_name() const	{ return "CastorEncryptACKAuth"; }
		const char *port_count() const	{ return PORTS_1_1; }
		const char *processing() const	{ return PUSH; }
		int configure(Vector<String>&, ErrorHandler*);

		void push(int, Packet *);

	private:
		Crypto* _crypto;
};

CLICK_ENDDECLS

#endif
