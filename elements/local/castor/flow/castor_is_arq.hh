#ifndef CLICK_CASTOR_IS_ARQ_HH
#define CLICK_CASTOR_IS_ARQ_HH

#include <click/element.hh>

CLICK_DECLS

/**
 * Checks if the Castor v2 ARQ flag is set in the PKT.
 * output 0: ARQ is not set
 * output 1: ARQ is set
 */
class CastorIsARQ : public Element {
public:
	const char *class_name() const { return "CastorIsARQ"; }
	const char *port_count() const { return "1/2"; }
	const char *processing() const { return PROCESSING_A_AH; }

	Packet* simple_action(Packet *);
};

CLICK_ENDDECLS

#endif
