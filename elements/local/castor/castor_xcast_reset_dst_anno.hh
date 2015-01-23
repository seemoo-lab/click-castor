#ifndef CLICK_CASTOR_XCAST_RESET_DST_ANNO_HH
#define CLICK_CASTOR_XCAST_RESET_DST_ANNO_HH

#include <click/element.hh>
#include "castor.hh"

CLICK_DECLS

class CastorXcastResetDstAnno : public Element {
	public:
		CastorXcastResetDstAnno();
		
		const char *class_name() const	{ return "CastorXcastResetDstAnno"; }
		const char *port_count() const	{ return PORTS_1_1; }
		const char *processing() const	{ return PUSH; }
		int configure(Vector<String>&, ErrorHandler*);
		
		void push(int, Packet *);

	private:
		bool promisc;
};

CLICK_ENDDECLS
#endif
