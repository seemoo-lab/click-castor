#ifndef CLICK_CASTOR_RESET_DST_ANNO_HH
#define CLICK_CASTOR_RESET_DST_ANNO_HH

#include <click/element.hh>
#include "castor.hh"

CLICK_DECLS

class CastorResetDstAnno : public Element {
public:
	CastorResetDstAnno() {}

	const char *class_name() const { return "CastorResetDstAnno"; }
	const char *port_count() const { return PORTS_1_1; }
	const char *processing() const { return PUSH; }

	void push(int, Packet *);
};

CLICK_ENDDECLS
#endif
