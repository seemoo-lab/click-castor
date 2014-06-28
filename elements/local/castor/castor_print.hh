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
		IPAddress _address;
		String _label;
		bool _fullpkt;

};

CLICK_ENDDECLS

#endif
