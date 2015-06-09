#ifndef CLICK_CASTOR_AUTHENTICATE_FLOW_HH
#define CLICK_CASTOR_AUTHENTICATE_FLOW_HH

#include <click/element.hh>
#include "castor.hh"
#include "crypto/crypto.hh"

CLICK_DECLS

/**
 * Flow validation takes place at every intermediate node
 */
class CastorAuthenticateFlow : public Element {
	public:
		CastorAuthenticateFlow();
		
		const char *class_name() const	{ return "CastorAuthenticateFlow"; }
		const char *port_count() const	{ return "1/2"; }
		const char *processing() const	{ return PUSH; }
		int configure(Vector<String>&, ErrorHandler*);
		
		void push(int, Packet *);
	private:
		Crypto* crypto;
};

CLICK_ENDDECLS

#endif
