#ifndef CLICK_CASTORADDTOHISTORY_HH
#define CLICK_CASTORADDTOHISTORY_HH
#include <click/element.hh>
#include "castor.hh"
#include "castor_history.hh"

CLICK_DECLS

class CastorAddToHistory : public Element {
	public:
		CastorAddToHistory();
		~CastorAddToHistory();

		const char *class_name() const	{ return "CastorAddToHistory"; }
		const char *port_count() const	{ return "1/1"; }
		const char *processing() const	{ return PUSH; }
		int configure(Vector<String>&, ErrorHandler*);

	private:
		void push(int, Packet *);
		CastorHistory* history;
};

CLICK_ENDDECLS
#endif
