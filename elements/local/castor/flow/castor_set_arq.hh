#ifndef CLICK_CASTOR_SET_ARQ_HH
#define CLICK_CASTOR_SET_ARQ_HH

#include <click/element.hh>

CLICK_DECLS

/**
 * Sets or unsets the Castor v2 ARQ flag
 */
class CastorSetARQ : public Element {
public:
	const char *class_name() const { return "CastorSetARQ"; }
	const char *port_count() const { return PORTS_1_1; }
	const char *processing() const { return AGNOSTIC; }
	int configure(Vector<String>&, ErrorHandler*);

	Packet* simple_action(Packet *);
private:
	bool _arq;
};

CLICK_ENDDECLS

#endif
