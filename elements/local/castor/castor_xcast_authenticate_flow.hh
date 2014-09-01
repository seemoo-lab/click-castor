#ifndef CLICK_CASTOR_XCAST_AUTHENTICATE_FLOW_HH
#define CLICK_CASTOR_XCAST_AUTHENTICATE_FLOW_HH

#include <click/element.hh>
#include "castor.hh"
#include "crypto.hh"

CLICK_DECLS

/**
 * Flow validation takes place at every intermediate node
 */
class CastorXcastAuthenticateFlow : public Element {
	public:
		CastorXcastAuthenticateFlow();
		~CastorXcastAuthenticateFlow();
		
		const char *class_name() const	{ return "CastorXcastAuthenticateFlow"; }
		const char *port_count() const	{ return "1/2"; }
		const char *processing() const	{ return PUSH; }
		int configure(Vector<String>&, ErrorHandler*);
		
		void push(int, Packet *);
	private:
		Crypto* crypto;
};

CLICK_ENDDECLS

#endif
