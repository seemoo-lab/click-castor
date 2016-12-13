#ifndef CLICK_CASTOR_DEC_TTL_HH
#define CLICK_CASTOR_DEC_TTL_HH

#include <click/element.hh>

CLICK_DECLS

class CastorDecTtl : public Element {
public:
	const char *class_name() const { return "CastorDecTtl"; }
	const char *port_count() const { return "1/2"; }
	const char *processing() const { return PROCESSING_A_AH; }

	Packet* simple_action(Packet *);
};

CLICK_ENDDECLS

#endif
