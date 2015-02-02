#ifndef CLICK_CASTORPRINT_HH
#define CLICK_CASTORPRINT_HH

#include <click/element.hh>
#include "castor.hh"

CLICK_DECLS

class CastorPrint : public Element { 
	public:
		CastorPrint() : verbose(false) {}
		
		const char *class_name() const	{ return "CastorPrint"; }
		const char *port_count() const	{ return PORTS_1_1; }
		const char *processing() const	{ return PUSH; }
		int configure(Vector<String>&, ErrorHandler*);
		
		void push(int, Packet *);

	private:
		NodeId myId;
		String label;
		bool verbose;

};

CLICK_ENDDECLS

#endif
