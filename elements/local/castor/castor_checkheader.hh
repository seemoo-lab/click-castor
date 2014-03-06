#ifndef CLICK_CASTORCHECKHEADER_HH
#define CLICK_CASTORCHECKHEADER_HH
#include <click/element.hh>
#include "castor.hh"
CLICK_DECLS

class CastorCheckHeader : public Element { 
	public:
		CastorCheckHeader();
		~CastorCheckHeader();
		
		const char *class_name() const	{ return "CastorCheckHeader"; }
		const char *port_count() const	{ return "1/3"; }
		const char *processing() const	{ return PUSH; }
		int configure(Vector<String>&, ErrorHandler*);
		
		void push(int, Packet *);
	private:
		uint8_t 	getCastorType(const unsigned char *);
};

CLICK_ENDDECLS
#endif