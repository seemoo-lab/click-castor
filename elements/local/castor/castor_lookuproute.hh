#ifndef CLICK_CASTORLOOKUPROUTE_HH
#define CLICK_CASTORLOOKUPROUTE_HH
#include <click/element.hh>
#include "castor.hh"
#include "castor_routingtable.hh"

CLICK_DECLS

class CastorLookupRoute : public Element {
	public:
		CastorLookupRoute();
		~CastorLookupRoute();
		
		const char *class_name() const	{ return "CastorLookupRoute"; }
		const char *port_count() const	{ return "1/1"; }
		const char *processing() const	{ return PUSH; }
		int configure(Vector<String>&, ErrorHandler*);
		
		void push(int, Packet *);
	private:
		CastorRoutingTable* _table;
};

CLICK_ENDDECLS
#endif
