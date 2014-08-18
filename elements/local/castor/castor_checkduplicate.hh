#ifndef CLICK_CASTOR_CHECKDUPLICATE_HH
#define CLICK_CASTOR_CHECKDUPLICATE_HH

#include <click/element.hh>
#include "castor.hh"
#include "castor_history.hh"

CLICK_DECLS

class CastorCheckDuplicate : public Element {
	public:
		CastorCheckDuplicate();
		~CastorCheckDuplicate();

		const char *class_name() const	{ return "CastorCheckDuplicate"; }
		const char *port_count() const	{ return "1/3"; }
		const char *processing() const	{ return PUSH; }
		int configure(Vector<String>&, ErrorHandler*);

	private:
		void push(int, Packet *);
		CastorHistory* history;
};

CLICK_ENDDECLS

#endif
