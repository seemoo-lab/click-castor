#ifndef CLICK_CASTOR_CALC_ICV_HH
#define CLICK_CASTOR_CALC_ICV_HH

#include <click/element.hh>
#include "../crypto/crypto.hh"

CLICK_DECLS

class CastorCalcICV : public Element {
public:
	const char *class_name() const	{ return "CastorCalcICV"; }
	const char *port_count() const	{ return PORTS_1_1; }
	const char *processing() const	{ return AGNOSTIC; }
	int configure(Vector<String>&, ErrorHandler*);

	Packet* simple_action(Packet *);
private:
	Crypto* crypto;
};

CLICK_ENDDECLS

#endif
