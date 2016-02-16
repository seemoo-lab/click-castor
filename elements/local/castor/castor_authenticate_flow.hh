#ifndef CLICK_CASTOR_AUTHENTICATE_FLOW_HH
#define CLICK_CASTOR_AUTHENTICATE_FLOW_HH

#include <click/element.hh>
#include "castor_flow_table.hh"
#include "crypto/crypto.hh"

CLICK_DECLS

/**
 * Flow validation takes place at every intermediate node
 */
class CastorAuthenticateFlow : public Element {
public:
	const char *class_name() const { return "CastorAuthenticateFlow"; }
	const char *port_count() const { return "1/1-3"; }
	const char *processing() const { return PROCESSING_A_AH; }
	int configure(Vector<String>&, ErrorHandler*);

	Packet* simple_action(Packet *);
private:
	CastorFlowTable* flowtable;
	Crypto* crypto;
};

CLICK_ENDDECLS

#endif
