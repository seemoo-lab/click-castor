#ifndef CLICK_CASTOR_VALIDATE_FLOW_ATDEST_HH
#define CLICK_CASTOR_VALIDATE_FLOW_ATDEST_HH

#include <click/element.hh>
#include "castor.hh"
#include "crypto.hh"

CLICK_DECLS

// XXX Should be named CastorAuthenticatePkt

/**
 * Flow validation takes place at the destination node
 */
class CastorValidateFlowAtDestination : public Element {
	public:
		CastorValidateFlowAtDestination();
		~CastorValidateFlowAtDestination();
		
		const char *class_name() const	{ return "CastorValidateFlowAtDestination"; }
		const char *port_count() const	{ return "1/2"; }
		const char *processing() const	{ return PUSH; }
		int configure(Vector<String>&, ErrorHandler*);
		
		void push(int, Packet *);
	private:
		Crypto* crypto;
};

CLICK_ENDDECLS
#endif
