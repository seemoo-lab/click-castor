#ifndef CLICK_CASTOR_ADD_PKT_TOHISTORY_HH
#define CLICK_CASTOR_ADD_PKT_TOHISTORY_HH
#include <click/element.hh>
#include "castor.hh"
#include "castor_history.hh"

CLICK_DECLS

class CastorAddPKTToHistory: public Element {
public:
	CastorAddPKTToHistory();
	~CastorAddPKTToHistory();

	const char *class_name() const { return "CastorAddPKTToHistory"; }
	const char *port_count() const { return PORTS_1_1; }
	const char *processing() const { return PUSH; }
	int configure(Vector<String>&, ErrorHandler*);

	void push(int, Packet *);
private:
	CastorHistory* history;
};

CLICK_ENDDECLS
#endif
