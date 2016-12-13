#ifndef CLICK_CASTOR_IS_DBG_HH
#define CLICK_CASTOR_IS_DBG_HH

#include <click/element.hh>

CLICK_DECLS

class CastorIsDbg : public Element {
public:
	const char *class_name() const { return "CastorIsDbg"; }
	const char *port_count() const { return "1/2"; }
	const char *processing() const { return PROCESSING_A_AH; }

	Packet* simple_action(Packet *);
};

CLICK_ENDDECLS

#endif
