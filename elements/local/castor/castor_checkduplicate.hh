#ifndef CLICK_CASTORCHECKDUPLICATE_HH
#define CLICK_CASTORCHECKDUPLICATE_HH
#include <click/element.hh>
#include "castor.hh"
#include "castor_history.hh"

CLICK_DECLS

class CastorCheckDuplicate : public Element {
	public:
		CastorCheckDuplicate();
		~CastorCheckDuplicate();

		const char *class_name() const	{ return "CastorCheckDuplicate"; }
		const char *port_count() const	{ return "1/2"; }
		const char *processing() const	{ return PUSH; }
		int configure(Vector<String>&, ErrorHandler*);

	private:
		void push(int, Packet *);
		CastorHistory* _history;
};

CLICK_ENDDECLS
#endif
