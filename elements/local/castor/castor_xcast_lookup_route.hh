#ifndef CLICK_CASTOR_XCAST_LOOKUP_ROUTE_HH
#define CLICK_CASTOR_XCAST_LOOKUP_ROUTE_HH

#include <click/element.hh>
#include "castor.hh"
#include "castor_routingtable.hh"

CLICK_DECLS

class CastorXcastLookupRoute : public Element {
	public:
		CastorXcastLookupRoute();
		~CastorXcastLookupRoute();
		
		const char *class_name() const	{ return "CastorXcastLookupRoute"; }
		const char *port_count() const	{ return "1/1"; }
		const char *processing() const	{ return PUSH; }
		int configure(Vector<String>&, ErrorHandler*);
		
		void push(int, Packet *);
	private:
		CastorRoutingTable* _table;
};

CLICK_ENDDECLS
#endif
