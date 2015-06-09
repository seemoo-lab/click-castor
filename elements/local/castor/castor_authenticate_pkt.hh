#ifndef CLICK_CASTOR_AUTHENTICATE_PKT_HH
#define CLICK_CASTOR_AUTHENTICATE_PKT_HH

#include <click/element.hh>
#include "castor.hh"
#include "crypto/crypto.hh"

CLICK_DECLS

/**
 * Flow validation takes place at the destination node
 */
class CastorAuthenticatePkt : public Element {
	public:
		CastorAuthenticatePkt();
		~CastorAuthenticatePkt();
		
		const char *class_name() const	{ return "CastorAuthenticatePkt"; }
		const char *port_count() const	{ return "1/2"; }
		const char *processing() const	{ return PUSH; }
		int configure(Vector<String>&, ErrorHandler*);
		
		void push(int, Packet *);
	private:
		Crypto* crypto;
};

CLICK_ENDDECLS
#endif
