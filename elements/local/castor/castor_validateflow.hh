#ifndef CLICK_CASTORVALIDATEFLOW_HH
#define CLICK_CASTORVALIDATEFLOW_HH
#include <click/element.hh>
#include "castor.hh"
CLICK_DECLS

class CastorValidateFlow : public Element {
	public:
		CastorValidateFlow();
		~CastorValidateFlow();
		
		const char *class_name() const	{ return "CastorValidateFlow"; }
		const char *port_count() const	{ return "1/1"; }
		const char *processing() const	{ return PUSH; }
		//int configure(Vector<String>&, ErrorHandler*);
		
		void push(int, Packet *);
	private:
};

CLICK_ENDDECLS
#endif
