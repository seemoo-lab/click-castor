#ifndef CLICK_CASTORADDHEADER_HH
#define CLICK_CASTORADDHEADER_HH
#include <click/element.hh>
#include "castor.hh"
#include "castor_flow_stub.hh"
CLICK_DECLS

class CastorAddHeader : public Element { 
	public:
		CastorAddHeader();
		~CastorAddHeader();
		
		const char *class_name() const	{ return "CastorAddHeader"; }
		const char *port_count() const	{ return "1/1"; }
		const char *processing() const	{ return PUSH; }
		int configure(Vector<String>&, ErrorHandler*);
		
		void push(int, Packet *);

	protected:
		CastorFlowStub* cflow;
};

CLICK_ENDDECLS
#endif

