#ifndef CLICK_CASTORCREATEACK_HH
#define CLICK_CASTORCREATEACK_HH
#include <click/element.hh>
#include "castor.hh"
#include "crypto.hh"
CLICK_DECLS

class CastorCreateACK : public Element {
	public:
		CastorCreateACK();
		~CastorCreateACK();
		
		const char *class_name() const	{ return "CastorCreateACK"; }
		const char *port_count() const	{ return "1/2"; }
		const char *processing() const	{ return PUSH; }
		int configure(Vector<String>&, ErrorHandler*);
		
		void push(int, Packet *);
	private:
		Crypto* _crypto;
};

CLICK_ENDDECLS
#endif
