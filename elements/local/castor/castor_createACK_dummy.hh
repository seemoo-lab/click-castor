#ifndef CLICK_CastorCreateACKDummy_HH
#define CLICK_CastorCreateACKDummy_HH
#include <click/element.hh>
#include "castor.hh"
#include "crypto.hh"
CLICK_DECLS

class CastorCreateACKDummy : public Element {
	public:
		CastorCreateACKDummy();
		~CastorCreateACKDummy();
		
		const char *class_name() const	{ return "CastorCreateACKDummy"; }
		const char *port_count() const	{ return "1/2"; }
		const char *processing() const	{ return PUSH; }
		int configure(Vector<String>&, ErrorHandler*);
		
		void push(int, Packet *);
	private:
		Crypto* _crypto;
};

CLICK_ENDDECLS
#endif
