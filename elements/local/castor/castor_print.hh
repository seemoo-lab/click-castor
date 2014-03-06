#ifndef CLICK_CASTORPRINT_HH
#define CLICK_CASTORPRINT_HH
#include <click/element.hh>
#include "castor.hh"
CLICK_DECLS

class CastorPrint : public Element { 
	public:
		CastorPrint();
		~CastorPrint();
		
		const char *class_name() const	{ return "CastorPrint"; }
		const char *port_count() const	{ return "1/1"; }
		const char *processing() const	{ return PUSH; }
		int configure(Vector<String>&, ErrorHandler*);
		
		void push(int, Packet *);
	private:
		uint8_t 	getType(Packet *);
		bool 		getCastorPKTHeader(Packet *, Castor_PKT*);
		bool 		getCastorACKHeader(Packet *, Castor_ACK*);
		String 		hexToString(const unsigned char*, uint8_t );

		String _label;


};

CLICK_ENDDECLS
#endif
