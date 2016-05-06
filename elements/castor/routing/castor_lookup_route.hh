#ifndef CLICK_CASTOR_LOOKUP_ROUTE_HH
#define CLICK_CASTOR_LOOKUP_ROUTE_HH

#include <click/element.hh>
#include "castor_route_selector.hh"

CLICK_DECLS

class CastorLookupRoute : public Element {
public:
	const char *class_name() const	{ return "CastorLookupRoute"; }
	const char *port_count() const	{ return PORTS_1_1X2; }
	const char *processing() const	{ return PROCESSING_A_AH; }
	int configure(Vector<String>&, ErrorHandler*);

	Packet* simple_action(Packet *);
private:
	CastorRouteSelector* selector;
};

CLICK_ENDDECLS
#endif
