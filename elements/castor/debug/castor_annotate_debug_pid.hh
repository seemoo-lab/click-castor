#ifndef CLICK_CASTOR_ANNOTATE_DEBUG_PID_HH
#define CLICK_CASTOR_ANNOTATE_DEBUG_PID_HH

#include <click/element.hh>

CLICK_DECLS

class CastorAnnotateDebugPid: public Element {
public:
	const char *class_name() const { return "CastorAnnotateDebugPid"; }
	const char *port_count() const { return PORTS_1_1; }
	const char *processing() const { return AGNOSTIC; }

	Packet* simple_action(Packet *);
};

CLICK_ENDDECLS

#endif
