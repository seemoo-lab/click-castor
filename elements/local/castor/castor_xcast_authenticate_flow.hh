#ifndef CLICK_CASTOR_XCAST_AUTHENTICATE_FLOW_HH
#define CLICK_CASTOR_XCAST_AUTHENTICATE_FLOW_HH

#include <click/element.hh>
#include "crypto/crypto.hh"

CLICK_DECLS

/**
 * Flow validation takes place at every intermediate node
 */
class CastorXcastAuthenticateFlow : public Element {
public:
	const char *class_name() const	{ return "CastorXcastAuthenticateFlow"; }
	const char *port_count() const	{ return PORTS_1_1X2; }
	const char *processing() const	{ return PROCESSING_A_AH; }
	int configure(Vector<String>&, ErrorHandler*);

	Packet* simple_action(Packet *);
private:
	Crypto* crypto;
};

CLICK_ENDDECLS

#endif
