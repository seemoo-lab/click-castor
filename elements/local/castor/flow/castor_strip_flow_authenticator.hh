#ifndef CLICK_CASTOR_STRIP_FLOW_AUTHENTICATOR_HH
#define CLICK_CASTOR_STRIP_FLOW_AUTHENTICATOR_HH

#include <click/element.hh>

CLICK_DECLS

/**
 * Removes the variable-sized flow authenticator from PKT
 */
class CastorStripFlowAuthenticator : public Element {
public:
	const char *class_name() const { return "CastorStripFlowAuthenticator"; }
	const char *port_count() const { return PORTS_1_1; }
	const char *processing() const { return AGNOSTIC; }

	Packet* simple_action(Packet *);
};

CLICK_ENDDECLS

#endif
