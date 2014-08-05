#ifndef CLICK_CASTOR_XCAST_VALIDATE_FLOW_HH
#define CLICK_CASTOR_XCAST_VALIDATE_FLOW_HH

#include <click/element.hh>
#include "castor.hh"
#include "crypto.hh"

CLICK_DECLS

// XXX Should be named CastorXcastAuthenticateFlow

/**
 * Flow validation takes place at every intermediate node
 */
class CastorXcastValidateFlow : public Element {
	public:
		CastorXcastValidateFlow();
		~CastorXcastValidateFlow();
		
		const char *class_name() const	{ return "CastorXcastValidateFlow"; }
		const char *port_count() const	{ return "1/2"; }
		const char *processing() const	{ return PUSH; }
		int configure(Vector<String>&, ErrorHandler*);
		
		void push(int, Packet *);
	private:
		Crypto* crypto;
};

CLICK_ENDDECLS

#endif
