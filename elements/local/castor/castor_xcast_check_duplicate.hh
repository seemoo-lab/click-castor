#ifndef CLICK_CASTOR_XCAST_CHECK_DUPLICATE_HH
#define CLICK_CASTOR_XCAST_CHECK_DUPLICATE_HH

#include <click/element.hh>
#include "castor.hh"
#include "castor_history.hh"

CLICK_DECLS

class CastorXcastCheckDuplicate : public Element {
	public:
		CastorXcastCheckDuplicate();
		~CastorXcastCheckDuplicate();

		const char *class_name() const	{ return "CastorXcastCheckDuplicate"; }
		const char *port_count() const	{ return "1/3"; }
		const char *processing() const	{ return PUSH; }
		int configure(Vector<String>&, ErrorHandler*);

	private:
		void push(int, Packet *);
		CastorHistory* history;
		IPAddress myAddr;
};

CLICK_ENDDECLS

#endif
