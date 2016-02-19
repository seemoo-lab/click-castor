#ifndef CLICK_CASTOR_ADD_XCAST_PKT_TOHISTORY_HH
#define CLICK_CASTOR_ADD_XCAST_PKT_TOHISTORY_HH

#include <click/element.hh>
#include "castor_history.hh"

CLICK_DECLS

class CastorAddXcastPktToHistory: public Element {
public:
	CastorAddXcastPktToHistory() : history(NULL) {}

	const char *class_name() const { return "CastorAddXcastPktToHistory"; }
	const char *port_count() const { return PORTS_1_1; }
	const char *processing() const { return PUSH; }
	int configure(Vector<String>&, ErrorHandler*);

	void push(int, Packet *);
private:
	CastorHistory* history;
};

CLICK_ENDDECLS

#endif
